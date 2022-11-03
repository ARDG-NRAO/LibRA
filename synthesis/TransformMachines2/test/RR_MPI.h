
#define UI_MAX_STRING_LEN 48
#define UI_MAX_PATH_LEN 1024

struct UIParams {
  char MSNBuf[UI_MAX_PATH_LEN];
  char cfCache[UI_MAX_PATH_LEN];
  char imageName[UI_MAX_PATH_LEN];
  char modelImageName[UI_MAX_PATH_LEN];
  char cmplxGridName[UI_MAX_PATH_LEN];

  char ftmName[UI_MAX_STRING_LEN];
  char fieldStr[UI_MAX_STRING_LEN];
  char spwStr[UI_MAX_STRING_LEN];
  char uvDistStr[UI_MAX_STRING_LEN];
  char phaseCenter[UI_MAX_STRING_LEN];
  char stokes[UI_MAX_STRING_LEN];
  char refFreqStr[UI_MAX_STRING_LEN];
  char weighting[UI_MAX_STRING_LEN];
  char rmode[UI_MAX_STRING_LEN];
  char sowImageExt[UI_MAX_STRING_LEN];
  char imagingMode[UI_MAX_STRING_LEN];

  int NX;
  int nW;
  int nInt;
  int nchan;
  int msInflationFactor;

  bool WBAwp;
  bool restartUI;
  bool doPointing;
  bool normalize;
  bool doPBCorr;
  bool conjBeams;
  bool doQuit;
  bool doSPWDataIter;

  float robust;
  float cellSize;
  float pbLimit;
  float posigdev[2];

  UIParams() {}

  UIParams(
    string _MSNBuf,
    string _ftmName,
    string _cfCache,
    string _fieldStr,
    string _spwStr,
    string _uvDistStr,
    string _imageName,
    string _modelImageName,
    string _cmplxGridName,
    string _phaseCenter,
    string _stokes,
    string _refFreqStr,
    string _weighting,
    string _rmode,
    float _robust,
    string _sowImageExt,
    string _imagingMode,
    float _cellSize,
    int _NX,
    int _nW,
    int _nInt,
    int _nchan,
    Bool _WBAwp,
    Bool _restartUI,
    Bool _doPointing,
    Bool _normalize,
    Bool _doPBCorr,
    Bool _conjBeams,
    Float _pbLimit,
    vector<float> _posigdev,
    Bool _doSPWDataIter,
    int _msInflationFactor)
  :
   NX(_NX)
  , nW(_nW)
  , nInt(_nInt)
  , nchan(_nchan)
  , msInflationFactor(_msInflationFactor)
  , WBAwp(_WBAwp)
  , restartUI(_restartUI)
  , doPointing(_doPointing)
  , normalize(_normalize)
  , doPBCorr(_doPBCorr)
  , conjBeams(_conjBeams)
  , doQuit(false)
  , doSPWDataIter(_doSPWDataIter)
  , robust(_robust)
  , cellSize(_cellSize)
  , pbLimit(_pbLimit){

#define CPY_STR(d, s) do {                      \
      strncpy(d, s.c_str(), sizeof(d));         \
      d[sizeof(d) - 1] = '\0';                  \
    } while(0)
    CPY_STR(MSNBuf, _MSNBuf);
    CPY_STR(ftmName, _ftmName);
    CPY_STR(cfCache, _cfCache);
    CPY_STR(fieldStr, _fieldStr);
    CPY_STR(spwStr, _spwStr);
    CPY_STR(uvDistStr, _uvDistStr);
    CPY_STR(imageName, _imageName);
    CPY_STR(modelImageName, _modelImageName);
    CPY_STR(cmplxGridName, _cmplxGridName);
    CPY_STR(phaseCenter, _phaseCenter);
    CPY_STR(stokes, _stokes);
    CPY_STR(refFreqStr, _refFreqStr);
    CPY_STR(weighting, _weighting);
    CPY_STR(rmode, _rmode);
    CPY_STR(sowImageExt, _sowImageExt);
    CPY_STR(imagingMode, _imagingMode);
#undef CPY_STR
    posigdev[0] = _posigdev[0];
    posigdev[1] = _posigdev[1];
  };

  void
  unpack(
    string& _MSNBuf,
    string& _ftmName,
    string& _cfCache,
    string& _fieldStr,
    string& _spwStr,
    string& _uvDistStr,
    string& _imageName,
    string& _modelImageName,
    string& _cmplxGridName,
    string& _phaseCenter,
    string& _stokes,
    string& _refFreqStr,
    string& _weighting,
    string& _rmode,
    float& _robust,
    string& _sowImageExt,
    string& _imagingMode,
    float& _cellSize,
    int& _NX,
    int& _nW,
    int& _nInt,
    int& _nchan,
    Bool& _WBAwp,
    Bool& _restartUI,
    Bool& _doPointing,
    Bool& _normalize,
    Bool& _doPBCorr,
    Bool& _conjBeams,
    Float& _pbLimit,
    vector<float>& _posigdev,
    Bool& _doSPWDataIter,
    int& _msInflationFactor) {

    _MSNBuf = string(MSNBuf);
    _ftmName = string(ftmName);
    _cfCache = string(cfCache);
    _fieldStr = string(fieldStr);
    _spwStr = string(spwStr);
    _uvDistStr = string(uvDistStr);
    _imageName = string(imageName);
    _modelImageName = string(modelImageName);
    _cmplxGridName = string(cmplxGridName);
    _phaseCenter = string(phaseCenter);
    _stokes = string(stokes);
    _refFreqStr = string(refFreqStr);
    _weighting = string(weighting);
    _rmode = string(rmode);
    _sowImageExt = string(sowImageExt);
    _imagingMode = string(imagingMode);
    _robust = robust;
    _cellSize = cellSize;
    _NX = NX;
    _nW = nW;
    _nInt = nInt;
    _nchan = nchan;
    _WBAwp = WBAwp;
    _restartUI = restartUI;
    _doPointing = doPointing;
    _normalize = normalize;
    _doPBCorr = doPBCorr;
    _conjBeams = conjBeams;
    _pbLimit = pbLimit;
    _posigdev.resize(2);
    _posigdev[0] = posigdev[0];
    _posigdev[1] = posigdev[1];
    _doSPWDataIter = doSPWDataIter;
    _msInflationFactor = msInflationFactor;
  }

#ifdef ROADRUNNER_USE_MPI
  static MPI_Datatype datatype;
  static MPI_Datatype compute_datatype()
    {
      int blocklengths[4] =
        {
          5 * UI_MAX_PATH_LEN + 11 * UI_MAX_STRING_LEN, // chars
          5, // ints
          8, // bools
          5  // floats
        };
      MPI_Aint displacements[4] =
        {
          0,
          offsetof(UIParams, NX),
          offsetof(UIParams, WBAwp),
          offsetof(UIParams, robust)
        };
      MPI_Datatype types[4] =
        {
          MPI_CHAR,
          MPI_INT,
          MPI_C_BOOL,
          MPI_FLOAT
        };
      MPI_Datatype result;
      MPI_Type_create_struct(4, blocklengths, displacements, types, &result);
      return result;
    }
#else
  static int datatype;
  static int compute_datatype()
    {
      return 0;
    }
#endif

};

#ifdef ROADRUNNER_USE_MPI
MPI_Datatype UIParams::datatype;

void mpi_init(int* argc, char*** argv)
{
  const int req = MPI_THREAD_FUNNELED;
  int prov = -1;
  MPI_Init_thread(argc, argv, req, &prov);
  if (prov < req)
    {
      std::cerr << "Available MPI thread level ("
                << prov
                << ") is lower than required level ("
                << req
                << ")"
                << std::endl;
      exit(1);
  }
}

void mpi_finalize()
{
  MPI_Finalize();
}

int mpi_comm_size(MPI_Comm comm, int* size)
{
  return MPI_Comm_size(comm, size);
}

int mpi_comm_rank(MPI_Comm comm, int* rank)
{
  return MPI_Comm_rank(comm, rank);
}

int mpi_barrier(MPI_Comm comm)
{
  return MPI_Barrier(comm);
}

int mpi_bcast(
  void *buffer, int count, MPI_Datatype datatype, int root,
  MPI_Comm comm)
{
  return MPI_Bcast(buffer, count, datatype, root, comm);
}

int mpi_reduce(
  const void* sendbuf, void* recvbuf, int count,
  MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
  return MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
}

int mpi_type_commit(MPI_Datatype* dt)
{
  return MPI_Type_commit(dt);
}

int mpi_type_free(MPI_Datatype* dt)
{
  return MPI_Type_free(dt);
}

#ifdef ROADRUNNER_USE_HPG
template <typename T>
struct mpi_datatype {
  static MPI_Datatype constexpr value = MPI_DATATYPE_NULL;
};
template <>
struct mpi_datatype<std::complex<double>> {
  static MPI_Datatype constexpr value = MPI_C_DOUBLE_COMPLEX;
};
template <>
struct mpi_datatype<std::complex<float>> {
  static MPI_Datatype constexpr value = MPI_C_COMPLEX;
};
template <>
struct mpi_datatype<double> {
  static MPI_Datatype constexpr value = MPI_DOUBLE;
};
template <>
struct mpi_datatype<float> {
  static MPI_Datatype constexpr value = MPI_FLOAT;
};

constexpr MPI_Datatype gridValueDatatype =
  mpi_datatype<hpg::GridValueArray::value_type>::value;
constexpr MPI_Datatype gridWeightDatatype =
  mpi_datatype<hpg::GridWeightArray::value_type>::value;
#else // !ROADRUNNER_USE_HPG
// TODO: can this be determined from a CASA data type definition?
constexpr MPI_Datatype gridValueDatatype = MPI_C_DOUBLE_COMPLEX;
constexpr MPI_Datatype gridWeightDatatype = MPI_DOUBLE;
#endif // ROADRUNNER_USE_HPG

#else // !ROADRUNNER_USE_MPI
int UIParams::datatype;

void mpi_init(int*, char***)
{
  // no-op
}

void mpi_finalize()
{
  // no-op
}

int mpi_comm_size(int, int* size)
{
  *size = 1;
  return 0;
}

int mpi_comm_rank(int, int* rank)
{
  *rank = 0;
  return 0;
}

int mpi_barrier(int)
{
  // no-op
  return 0;
}

int mpi_bcast(void *, int, int, int, int)
{
  // no-op
  return 0;
}

int mpi_reduce(const void*, void*, size_t, int, int, int, int)
{
  // no-op
  return 0;
}

int mpi_type_commit(int*)
{
  // no-op
  return 0;
}

int mpi_type_free(int*)
{
  // no-op
  return 0;
}
#define MPI_COMM_WORLD 0
#define MPI_IN_PLACE nullptr
#define MPI_SUM 0
#define MPI_UNSIGNED_LONG 0
int const gridValueDatatype = 0;
int const gridWeightDatatype = 0;

#endif // ROADRUNNER_USE_MPI
