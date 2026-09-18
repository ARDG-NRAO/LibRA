#!/usr/bin/env bash
# Local staged container build and test script using podman.
# Mirrors the CI pipeline: base -> deps -> components -> tests
# Each stage is built, optionally pushed, before the next starts.
#
# Usage:
#   ./scripts/local_container_build.sh [OPTIONS]
#
# Options:
#   --stage <base|deps|components|tests|all>  Which stage(s) to build (default: all)
#   --arch  <AMPERE80|VOLTA70|HOPPER90>       Kokkos CUDA arch (default: AMPERE80)
#   --jobs  <N>                               Parallel build jobs (default: nproc)
#   --push                                    Push each stage after building
#   --registry <registry>                     Registry prefix (default: ardgnrao)
#   --branch <branch>                         LibRA branch to clone in base (default: current)
#   --no-cache                                Pass --no-cache to podman build
#   --run-tests                               Run tests after building components
#   -h, --help                                Show this help

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
RECIPE_DIR="${REPO_ROOT}/scripts/container_recipes/CI"

# Arch -> CUDA version mapping (V100 tops out at 12.9.1)
declare -A ARCH_CUDA_VERSION
ARCH_CUDA_VERSION[VOLTA70]="12.9.1"
ARCH_CUDA_VERSION[AMPERE80]="13.0.0"
ARCH_CUDA_VERSION[AMPERE86]="13.0.0"
ARCH_CUDA_VERSION[HOPPER90]="13.0.0"

# Defaults
STAGE="all"
KOKKOS_ARCH="VOLTA70"
JOBS="$(nproc)"
PUSH=false
REGISTRY="ardgnrao"
BRANCH="$(git -C "${REPO_ROOT}" rev-parse --abbrev-ref HEAD 2>/dev/null || echo main)"
NO_CACHE=""
RUN_TESTS=false
GITLAB_TOKEN="${GITLAB_TOKEN:-}"  # read from env; set before calling script

usage() {
  grep '^#' "$0" | grep -v '#!/' | sed 's/^# \?//'
  exit 0
}

while [[ $# -gt 0 ]]; do
  case $1 in
    --stage)     STAGE="$2";      shift 2 ;;
    --arch)      KOKKOS_ARCH="$2"; shift 2 ;;
    --jobs)      JOBS="$2";       shift 2 ;;
    --push)      PUSH=true;       shift   ;;
    --registry)  REGISTRY="$2";  shift 2 ;;
    --branch)    BRANCH="$2";    shift 2 ;;
    --no-cache)  NO_CACHE="--no-cache"; shift ;;
    --run-tests) RUN_TESTS=true; shift   ;;
    -h|--help)   usage ;;
    *) echo "Unknown option: $1"; exit 1 ;;
  esac
done

ARCH_TAG=$(echo "${KOKKOS_ARCH}" | tr '[:upper:]' '[:lower:]')
CUDA_VERSION="${ARCH_CUDA_VERSION[${KOKKOS_ARCH}]:-13.0.0}"

BASE_IMAGE="${REGISTRY}/libra-base:${ARCH_TAG}"
DEPS_IMAGE="${REGISTRY}/libra-deps:${ARCH_TAG}"
COMPONENTS_IMAGE="${REGISTRY}/libra-components:${ARCH_TAG}"
TESTS_IMAGE="${REGISTRY}/libra-tests:${ARCH_TAG}"

log()  { echo "[$(date '+%H:%M:%S')] $*"; }
fail() { echo "[ERROR] $*" >&2; exit 1; }

build_stage() {
  local name="$1"
  local dockerfile="$2"
  local tag="$3"
  shift 3
  local extra_args=("$@")

  # Registry-backed BuildKit cache, shared with the GitLab CI pipeline.
  # Read unconditionally (anonymous pull is fine for public repos);
  # write only when --push is set, since cache export needs credentials.
  local cache_ref="${tag}-buildcache"
  local cache_args=()
  if [[ -z "${NO_CACHE}" ]]; then
    cache_args+=(--cache-from "type=registry,ref=${cache_ref}")
    if ${PUSH}; then
      cache_args+=(--cache-to "type=registry,ref=${cache_ref},mode=max")
    fi
  fi

  log "=== Building stage: ${name} ==="
  podman build \
    ${NO_CACHE} \
    "${cache_args[@]}" \
    -f "${RECIPE_DIR}/${dockerfile}" \
    -t "${tag}" \
    "${extra_args[@]}" \
    "${RECIPE_DIR}" \
    || fail "Stage '${name}' failed"

  log "Stage '${name}' built successfully -> ${tag}"

  if ${PUSH}; then
    log "Pushing ${tag}..."
    podman push "${tag}" || fail "Push of ${tag} failed"
    log "Pushed ${tag}"
  fi
}

run_stage_base() {
  [[ -z "${GITLAB_TOKEN}" ]] && fail "GITLAB_TOKEN is not set. Export it or pass it: GITLAB_TOKEN=<pat> $0"
  build_stage "base" "Dockerfile.base" "${BASE_IMAGE}" \
    --build-arg "CUDA_VERSION=${CUDA_VERSION}" \
    --build-arg "LIBRA_BRANCH=${BRANCH}" \
    --build-arg "GITLAB_TOKEN=${GITLAB_TOKEN}"
}

run_stage_deps() {
  build_stage "deps" "Dockerfile.deps" "${DEPS_IMAGE}" \
    --build-arg "BASE_IMAGE=${BASE_IMAGE}" \
    --build-arg "CUDA_ARCH=${KOKKOS_ARCH}" \
    --build-arg "nproc=${JOBS}"
}

run_stage_components() {
  build_stage "components" "Dockerfile.components" "${COMPONENTS_IMAGE}" \
    --build-arg "nproc=${JOBS}"
}

run_stage_tests() {
  build_stage "tests" "Dockerfile.tests" "${TESTS_IMAGE}"

  if ${RUN_TESTS}; then
    log "=== Running tests ==="
    podman run --rm \
      --name libra-test-run \
      "${TESTS_IMAGE}" \
      || { log "Tests reported failures (see output above)"; exit 1; }
    log "Tests passed"
  fi
}

case "${STAGE}" in
  base)       run_stage_base ;;
  deps)       run_stage_deps ;;
  components) run_stage_components ;;
  tests)      run_stage_tests ;;
  all)
    run_stage_base
    run_stage_deps
    run_stage_components
    run_stage_tests
    ;;
  *) fail "Unknown stage '${STAGE}'. Use: base, deps, components, tests, all" ;;
esac

log "Done."
