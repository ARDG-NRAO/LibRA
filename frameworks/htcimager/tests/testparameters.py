_interface = 'libra_cl'
_jobmode = 'makePSF, makeDirtyImage'
_parfile = 'gridding.def'
_jobname = 'makePSF_n1'
_mslist = 'ms1.ms, ms2.ms'
_cfclist = 'onecfc.cf'

arglist = ['--interface', _interface,
           '-f', _parfile,
           '--jobmode', _jobmode,
           '-m', _mslist,
           '--cfclist', _cfclist,
           '--jobname', _jobname]
