import os
import re
import shutil
import subprocess

# import params.py
import sys
sys.path.append('.')


# definitions to initialize imager parameters to use with htclean components
try:
    from casatasks.private.casa_transition import *
    if is_CASA6:
        from casatasks.private.imagerhelpers.input_parameters import ImagerParameters
        from casatasks import mstransform
        import casatools
        tb = casatools.table()
    else:
        raise Exception('not CASA6, executing CASA5 imports')
except:
    from imagerhelpers.input_parameters import ImagerParameters
    from mstransform_cli import mstransform_cli as mstransform
    from casac import casac
    tb = casac.table()


def import_tclean_parameters(infile = 'tclean.last'):
    with open(infile) as paramsfile:
        pars = {}
        lines = paramsfile.readlines()
        for line in lines:
            if line != '\n' and '#' not in line[0]:
                line = line.split('=')
                pars[line[0].strip()] = eval(line[1].strip())

    # Deal with parameters that are not the same name
    keys = pars.keys()
    if 'vis' in keys: pars['msname'] = pars.pop('vis')
    if 'timerange' in keys: pars['timestr'] = pars.pop('timerange')
    if 'uvrange' in keys: pars['uvdist'] = pars.pop('uvrange')
    if 'observation' in keys: pars['obs'] = pars.pop('observation')
    if 'intent' in keys: pars['state'] = pars.pop('intent')
    if 'gain' in keys: pars['loopgain'] = pars.pop('gain')
    if 'smallscalebias' in keys: pars['scalebias'] = pars.pop('smallscalebias')

    # Delete parameters that do not exist in the context of ImagerParameters
    if 'outlierfile' in keys: del pars['outlierfile']
    if 'selectdata' in keys: del pars['selectdata']
    if 'restoration' in keys: del pars['restoration']
    if 'calcres' in keys: del pars['calcres']
    if 'calcpsf' in keys: del pars['calcpsf']
    if 'pbcor' in keys: del pars['pbcor']
    if 'psfphasecenter' in keys: del pars['psfphasecenter']
    if 'taskname' in keys: del pars['taskname']

    return pars


#def setImagerParameters(vis, field='', phasecenter, spw='', imagename, imsize, cell, datacolumn='data', cfcache='cfcache', gridder = 'mosaic', conjbeams = False, deconvolver = 'hogbom', scales=[0], weighting = 'natural', robust=0.5, uvtaper='', niter=0, gain=0.1, nsigma=2.0, cycleniter=5000, cyclefactor=1, mask='', savemodel="none", intent = '', uvrange = '', usepointing = False):
#    paramList =  ImagerParameters(
#        msname               = vis,
#        
#        datacolumn           = datacolumn,
#        imagename            = imagename,
#        imsize               = imsize,
#        cell                 = cell,
#        phasecenter          = phasecenter,
#        stokes               = 'I',
#        projection           = 'SIN',
#        startmodel           = '',
#        specmode             = 'mfs',
#        gridder              = gridder,
#        deconvolver          = deconvolver,
#        
#        outlierfile          = '',
#        weighting            = weighting,
#        niter                = niter,
#        usemask              = 'user',
#        fastnoise            = True,
#        restart              = True,
#        savemodel            = savemodel,
#        
#        
#        parallel             = False,
#        field                = field,
#        spw                  = spw,
#        
#        uvdist               = uvrange,
#        antenna              = '',
#        scan                 = '',
#        
#        state                = intent,
#        reffreq              = '',
#        nchan                = -1,
#        start                = '',
#        width                = '',
#        outframe             = 'LSRK',
#        veltype              = 'radio',
#        restfreq             = [],
#        interpolation        = 'linear',
#        perchanweightdensity = True,
#        facets               = 1,
#        
#        chanchunks           = 1,
#        wprojplanes          = wprojplanes,
#        vptable              = '',
#        mosweight            = False,
#        aterm                = True,
#        psterm               = False,
#        wbawp                = True,
#        conjbeams            = conjbeams,
#        cfcache              = cfcache,
#        usepointing          = usepointing,
#        computepastep        = 360.0,
#        rotatepastep         = 360.0,
#        pointingoffsetsigdev = pointingoffsetsigdev,
#        pblimit              = 0.2,
#        normtype             = 'flatnoise',
#        scales               = scales,
#        nterms               = 2,
#        scalebias            = 0.0,
#        restoringbeam        =  [],
#        
#        robust               = robust,
#        noise                = '1.0Jy',
#        npixels              = 0,
#        uvtaper              = uvtaper,
#        loopgain             = gain,
##        threshold            = 0.2,
#        nsigma               = nsigma,
#        cycleniter           = cycleniter,
#        cyclefactor          = cyclefactor,
#        minpsffraction       = 0.05,
#        maxpsffraction       = 0.8,
#        interactive          = False,
#        mask                 = mask,
#        pbmask               = 0.0,
#        sidelobethreshold    = 3.0,
#        noisethreshold       = 5.0,
#        lownoisethreshold    = 1.5,
#        negativethreshold    = 0.0,
#        smoothfactor         = 1.0,
#        minbeamfrac          = 0.3,
#        cutthreshold         = 0.01,
#        growiterations       = 75,
#        dogrowprune          = True,
#        minpercentchange     = -1.0,
#        verbose              = False,
#)
#    return paramList

def getRowRanges(msname,nparts):

    tb.open(msname);
    nrows=tb.nrows();
    nSubRows = int(nrows/nparts);
    tb.close();
    r=[];
    lastrow=nrows;
    rBeg=1;
    for i in range(nparts):
        rEnd = rBeg + nSubRows;
        if (rEnd > lastrow):
            rEnd=lastrow;
        r.append([rBeg, rEnd]);
        rBeg=rEnd+1;

    return r;


def getSPWrange(msname):
    tb.open(msname + '/SPECTRAL_WINDOW')
    SPWrange = tb.rownumbers()
    tb.close()

    return SPWrange


def partitionByRow(msname, nparts, dryrun=False):
    r = getRowRanges(msname, nparts);
    _msname = msname.split('/')[-1]
    _msname = _msname.partition('.ms')[0]

    msnames=[];
    print("Making {} partitions".format(str(nparts)))
    for i in range(len(r)):
        sBeg = str(r[i][0]);
        sEnd = str(r[i][1]);
        #        mslet=os.path.basename(msname)+"#"+sBeg+'-'+sEnd;
        mslet = _msname + '_n' + str(i+1) + '.ms'
        msnames.append(mslet);
        taqlStr="ROWNUMBER() >= "+sBeg+" && ROWNUMBER() <= "+sEnd;

        if (not dryrun):
            print(mslet + " taql=" + taqlStr);

            mstransform(vis=msname,outputvis=mslet,createmms=False,separationaxis="auto",numsubms="auto",tileshape=[0],field="",spw="",scan="",antenna="",correlation="",timerange="",intent="",array="",uvrange="",observation="",feed="",datacolumn="data",realmodelcol=False,keepflags=True,usewtspectrum=False,combinespws=False,chanaverage=False,chanbin=1,hanning=False,regridms=False,mode="channel",nchan=-1,start=0,width=1,nspw=1,interpolation="linear",phasecenter="",restfreq="",outframe="",veltype="radio",preaverage=False,timeaverage=False,timebin="0s",timespan="",maxuvwdistance=0.0,docallib=False,callib="",douvcontsub=False,fitspw="",fitorder=0,want_cont=False,denoising_lib=True,nthreads=1,niter=1,disableparallel=False,ddistart=-1,taql=taqlStr,monolithic_processing=True,reindex=False);


def partitionBySPW(msname):
    msnames = []
    _msname = msname.partition('/')[-1]
    _msname = _msname.partition('.ms')[0]
    SPWrange = getSPWrange(msname)

    print('Partitioning the original MS by Spectral Window...')
    for i in SPWrange:
        spw = str(i)
        mslet = _msname + '_SPW' + str(i) + '.ms'
        msnames.append(mslet)

        print('{}, {}'.format(mslet, spw))
        mstransform(vis=msname,outputvis=mslet,createmms=False,separationaxis="auto",numsubms="auto",tileshape=[0],field="",spw=spw,scan="",antenna="",correlation="",timerange="",intent="",array="",uvrange="",observation="",feed="",datacolumn="data",realmodelcol=False,keepflags=True,usewtspectrum=False,combinespws=False,chanaverage=False,chanbin=1,hanning=False,regridms=False,mode="channel",nchan=-1,start=0,width=1,nspw=1,interpolation="linear",phasecenter="",restfreq="",outframe="",veltype="radio",preaverage=False,timeaverage=False,timebin="0s",timespan="",maxuvwdistance=0.0,docallib=False,callib="",douvcontsub=False,fitspw="",fitorder=0,want_cont=False,denoising_lib=True,nthreads=1,niter=1,disableparallel=False,ddistart=-1,taql='',monolithic_processing=True,reindex=False)


def partitionBySPW_Row(msname, nparts, dryrun=False):
    SPWrange = getSPWrange(msname)
    nSPW = len(SPWrange)    
    nprows = int(nparts / nSPW)		# correct to only accept integer multiples of nSPW
    r = getRowRanges(msname, nprows);
    _msname = msname.split('/')[-1]
    _msname = _msname.partition('.ms')[0]

    msnames=[];
    print("Making {} partitions".format(str(nparts)))
    for i, spw in enumerate(SPWrange):
        for j in range(nprows):
            sBeg = str(r[j][0]);
            sEnd = str(r[j][1]);

            mslet = _msname + '_n' + str((i * nprows) + j + 1) + '.ms'
            msnames.append(mslet);
            taqlStr="ROWNUMBER() >= "+sBeg+" && ROWNUMBER() <= "+sEnd;
    
            if (not dryrun):
                print(mslet + " taql=" + taqlStr);
    
                mstransform(vis=msname,outputvis=mslet,createmms=False,separationaxis="auto",numsubms="auto",tileshape=[0],field="",spw=str(spw),scan="",antenna="",correlation="",timerange="",intent="",array="",uvrange="",observation="",feed="",datacolumn="data",realmodelcol=False,keepflags=True,usewtspectrum=False,combinespws=False,chanaverage=False,chanbin=1,hanning=False,regridms=False,mode="channel",nchan=-1,start=0,width=1,nspw=1,interpolation="linear",phasecenter="",restfreq="",outframe="",veltype="radio",preaverage=False,timeaverage=False,timebin="0s",timespan="",maxuvwdistance=0.0,docallib=False,callib="",douvcontsub=False,fitspw="",fitorder=0,want_cont=False,denoising_lib=True,nthreads=1,niter=1,disableparallel=False,ddistart=-1,taql=taqlStr,monolithic_processing=True,reindex=False);


def partitionMS(msname, partaxis, nparts = 1, dryrun = False):
    if partaxis == 'spw':
        partitionBySPW(msname)
    elif partaxis == 'row':
        partitionByRow(msname, nparts = nparts, dryrun = dryrun)
    elif partaxis == 'spw-row':
        partitionBySPW_Row(msname, nparts = nparts, dryrun = dryrun)
    else:
        raise Exception("unknown parameter value: partaxis allowed values are 'spw', 'row', 'spw-row'.")
