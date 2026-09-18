
# LibRA Container Build

Container images for LibRA are built by this repo's own GitLab CI
(`.gitlab-ci.yml`, `container-*` jobs) and pushed to [Docker Hub](https://hub.docker.com/u/ardgnrao).

## Image tags

Images are tagged by CUDA architecture — there is no `:latest` tag. Pull the image matching your GPU:

| GPU | Tag |
|---|---|
| V100 | `volta70` |
| A100 | `ampere80` |
| H100 | `hopper90` |

Example:
```
singularity build libra.sif docker://ardgnrao/libra-components:volta70
```
or with podman/docker:
```
podman pull ardgnrao/libra-components:volta70
```

## Staged images

| Image | Purpose |
|---|---|
| `ardgnrao/libra-base:<arch>` | CUDA + system packages (CUDA version pinned per arch) |
| `ardgnrao/libra-deps:<arch>` | All external CMake dependencies built |
| `ardgnrao/libra-components:<arch>` | LibRA apps, libraries, frameworks |
| `ardgnrao/libra-tests:<arch>` | Test runner (C++ GTest + Python pytest) — volta70 only today |

## Running

```
singularity shell --nv ardgnrao/libra-components:volta70
Singularity> roadrunner
```

Bind additional host paths as needed:
```
singularity shell --nv --bind host_path:container_path ardgnrao/libra-components:volta70
```

## CUDA driver compatibility

See the [CUDA toolkit release notes](https://docs.nvidia.com/cuda/cuda-toolkit-release-notes/index.html) for the minimum driver version required for each toolkit version.
