#!/usr/bin/env python3

# Copyright (C) 2024
# Associated Universities, Inc. Washington DC, USA.
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.is
#
# You should have received a copy of the GNU Library General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
#
# Correspondence concerning this should be addressed as follows:
#        Postal address: National Radio Astronomy Observatory
#                        1003 Lopezville Road,
#                        Socorro, NM - 87801, USA
#
# $Id$

import os
import json

import htcondor2 as htcondor
from htcondor2 import dags


def flattenDictValues(x: dict) -> list:
    x1 = x.copy()
    if 'allkeys' in list(x1): del x1['allkeys']
    outlist = [y for sublist in x1.values() for y in sublist]
    return outlist


def writeGriddingVARS(msnamelist, cfcachelist, imgnamelist, msdir: str,
                      cfcachedir: str, jobmode: str, mindiskreq: str) -> list:
    varsList = []

    if 'G' in mindiskreq:
        mindiskreq = int(mindiskreq.replace('G','').strip())
    else:
        mindiskreq = int(mindiskreq)

    gridnorm = 'makepsfc2tt' if jobmode == 'makePSF' else 'runresidualc2tt'
    jobmodelist = []
    modelnamelist = []
    if type(msnamelist)  == dict:
        for i,key in enumerate(list(msnamelist)):
            if len(msnamelist[key]) == 1:
                jobmodelist.append(gridnorm)
            else:
                jobmodelist += [jobmode for x in msnamelist[key]]
            modelnamelist += [imgnamelist['allkeys'][i] for x in msnamelist[key]]
        msnamelist  = flattenDictValues(msnamelist)
    else:
        jobmodelist = [jobmode for x in msnamelist]
        baseimagename = os.path.commonprefix(imgnamelist)[:-2]
        modelnamelist += [f'{baseimagename}' for x in msnamelist]
    if type(cfcachelist) == dict: cfcachelist = flattenDictValues(cfcachelist)
    if type(imgnamelist) == dict: imgnamelist = flattenDictValues(imgnamelist)

    for i, infiles in enumerate(zip(msnamelist, cfcachelist, imgnamelist, modelnamelist)):
        msname, cfcache, imgname, modelname = infiles
        jobmode = jobmodelist[i]
        getsizeGB = lambda filepathstr: int(os.path.getsize(filepathstr)/(1024**3)) + 1
        try:
            mssize = getsizeGB(f'{msdir}/{msname}')
            cfsize = getsizeGB(f'{cfcachedir}/{cfcache}')
        except:
            mssize = 0
            cfsize = 0
        diskrq = 2 * (mssize + cfsize) + mindiskreq
        varsList.append({
            'msname': msname, 
            'cfcache': cfcache, 
            'diskResidual' : f'{diskrq} G', 
            'jobmode': jobmode, 
            'this_image': imgname, 
            'this_modelname': modelname
        })

    return varsList


def makePartialImageNameList(msnamelist, imagename: str):
    if type(msnamelist) == dict:
        imgnamelist = {}
        imgnamelist['allkeys'] = []

        #i = 0
        for key in list(msnamelist):
            this_mslist = msnamelist[key]
            if type(this_mslist) == list:
                #imgnamelist[key] = [f'{imagename}_n{i + j}' for j in range(len(this_mslist))]
                #i += len(this_mslist
                this_imagename = f'{imagename}_{key}'
                imgnamelist['allkeys'].append(this_imagename)
                if len(this_mslist) > 1:
                    imgnamelist[key] = [f'{this_imagename}_n{j}' for j in range(len(this_mslist))]
                else:
                    imgnamelist[key] = [f'{this_imagename}']
            else:
                raise TypeError(f'Dictionary values in msnamelist must be lists. Got {type(this_mslist)}.')
    else:
        imgnamelist = [f'{imagename}_n{i}' for i in range(len(msnamelist))]

    return imgnamelist


def convertCFListToDict(cflist : list, msdict : dict) -> dict:
    outputdict = {}

    for cf, key in zip(cflist, list(msdict)):
        outputdict[key] = [cf for x in msdict[key]]

    return outputdict


def getSupportImageList(baseimagename : str, nterms : int) -> list:
    supportImageList = []
    imtypes = ['pb', 'psf', 'sumwt']
    NTerms  = [1, 2 * nterms - 1 , 2 * nterms - 1]

    for imtype, numTerms in zip(imtypes, NTerms):
        this_imname = f'{baseimagename}.{imtype}'
        supportImageList.append(f'{this_imname}.tgz')
        for N in range(numTerms):
            imname = f'{this_imname}.tt{N}.tgz'
            supportImageList.append(imname)

    return supportImageList


def writeGatherVARS(imagename : str, imgnamelist, jobmode: str, multiterm: bool) -> list:
    varsList = []

    if jobmode == 'gatherPSF':
        imtypes = ['psf', 'sumwt', 'weight']
    else:
        imtypes = ['residual']

    inputfiles = lambda imglist, imtypelist: [f'$(imagesdir)/$(this_workdir)/{img}.{imtype}.tgz?pack=auto' for img in imglist for imtype in imtypelist]

    if type(imgnamelist) == dict:
        if multiterm:
            for key in list(imgnamelist):
                if key != 'allkeys':
                    this_imagelist = imgnamelist[key]
                    this_imagename = f'{imagename}_{key}'
                    if len(this_imagelist) > 1:
                        varsList.append({
                            'this_image'      : this_imagename,
                            'gatherimagelist' : ','.join(this_imagelist),
                            'inputfiles'      : ','.join(inputfiles(this_imagelist, imtypes))
                        })
        else:
            imgnamelist = flattenDictValues(imgnamelist)
            varsList.append({
                'this_image'      : imagename,
                'gatherimagelist' : ','.join(imgnamelist),
                'inputfiles'      : ','.join(inputfiles(imgnamelist, imtypes))
            })
    elif type(imgnamelist) == list:
        varsList.append({
            'this_image'      : imagename,
            'gatherimagelist' : ','.join(imgnamelist),
            'inputfiles'      : ','.join(inputfiles(imgnamelist, imtypes))
        })

    return varsList


def makeJobBaseDict(HTCpars: dict, IMGpars: dict) -> dict:
    jobdict = HTCpars.copy()
    del jobdict['min_disk_residual']
    htcimagerfiles = [
        'htcimager.py', 'GPUMonitor.py', 'customImager.py', 'inputArgs.py', 
        'jobmode.py', 'libra_cl_imager.py', 'makeImages.py', 'setupLibRA.py', 
        'staging.py']
    if 'disconnected_timeout' in list(jobdict):
        jobdict['job_lease_duration'] = jobdict.pop('disconnected_timeout')
    if 'min_pelican_version' in list(jobdict):
        jobdict['requirements'] = f"(PelicanPluginVersion >= {jobdict.pop('min_pelican_version')})"
    jobdict.update({
        'executable'            : 'htcimager.py',
        'transfer_executable'   : 'False',
        'should_transfer_files' : 'yes',
        'htcimagerfiles'        : ','.join([f'$(htcimagerdir)/{file}' for file in htcimagerfiles]),
        'request_cpus'          : '1',
        'use_oauth'             : 'nrao',
        '+WantIOProxy'          : 'True',
        'baseimagename'         : f"{IMGpars['baseimagename']}",
        'useStartMask'          : f"{IMGpars['useStartMask']}",
        'useStartModel'         : f"{IMGpars['useStartModel']}"
    })
    if 'allspwvis' in list(IMGpars):
        jobdict['allspwvis'] = f"{IMGpars['allspwvis']}"

    return jobdict


def createGriddingJob(jobbasedict   : dict, 
                      jobmode       : str, 
                      parfile       : str,
                      multiterm     : bool, 
                      imcycle       : int = 0) -> htcondor.Submit:
    
    jobdict = jobbasedict.copy()
    notgriddingparameters = ['memModel', 'min_disk_model', 'casa', 'useStartMask', 'allspwvis']
    for key in notgriddingparameters:
        if key in list(jobdict):
            del jobdict[key]
    memrequest = jobdict.pop('memResidual')
    wantOSPool = jobdict.pop('wantOSPool', 'False')
    undesired_sites = jobdict.pop('und_sites_grid', '')
    NRPexclusive = eval(jobdict.pop('NRPexclusive', 'False'))
    useStartModel = eval(jobdict.pop('useStartModel'))
    useVLASSGPUs = eval(jobdict.pop('useVLASSGPUs', 'False'))
    min_gpu_capability = jobdict.pop('min_gpu_capability')
    min_gpu_memory = jobdict.pop('min_gpu_memory')
    min_gpu_driver = jobdict.pop('min_gpu_driver')
    baseimagename = jobdict.pop('baseimagename')
    workdir_0 = f'{baseimagename}.workdirectory.imcycle0'
    this_workdir = f'{baseimagename}.workdirectory.imcycle{imcycle}'

    if NRPexclusive:
        jobdict['+DESIRED_SITES'] = '"SDSC-PRP"'

    if useVLASSGPUs:
        if 'requirements' in list(jobdict):
            jobdict['requirements'] += ' && (VLASS == True) && (HasLustre =!= True)'
        else:
            jobdict['requirements'] = '(VLASS == True) && (HasLustre =!= True)'
        jobdict['+partition'] = '"VLASS"'

    inputfiles   = [
        '$(htcimagerfiles)',
        '$(softwaredir)/$(libra_bundle)',
        '$(softwaredir)/casa-data.tgz?pack=auto',
        '$(cfcachedir)/$(cfcache).tgz?pack=auto',
        '$(msdir)/$(msname).tgz?pack=auto',
        f'bin/{parfile}'
    ]

    if jobmode == 'makePSF':
        imtypelist   = ['pb', 'psf', 'sumwt', 'weight']
        # defining transfer_output_files will only transfer those files, leaving it undefined transfers all new files,
        # allowing to use the same job definition for makePSF and makepsfc2tt (which also transfers .pb)
        #outputfiles  = ['logs.tgz'] + [f'$(this_image).{imtype}.tgz' for imtype in imtypelist]
        this_workdir = workdir_0
    else:
        if multiterm:
            inputfiles += [f'$(imagesdir)/{workdir_0}/$(this_image).{imtype}.tgz?pack=auto' for imtype in ['sumwt', 'weight']]
            if useStartModel or jobmode == 'runResidualCycle':
                inputfiles.append(f'$(imagesdir)/{this_workdir}/$(this_modelname).divmodel.tgz?pack=auto')
        else:
            if useStartModel or jobmode == 'runResidualCycle':
                inputfiles.append(f'$(imagesdir)/$(this_modelname).divmodel.imcycle{imcycle}.tgz?pack=auto')
        imtypelist   = ['residual']
        outputfiles  = ['logs.tgz'] + [f'$(this_image).{imtype}.tgz' for imtype in imtypelist]
        jobdict.update({
            'transfer_output_files'  : ','.join(outputfiles)
        })

    outputremaps = [f'logs.tgz = $(initialdir)/logs.$(JOB).tgz'] + [f'$(this_image).{imtype}.tgz = $(imagesdir)/{this_workdir}/$(this_image).{imtype}.tgz' for imtype in imtypelist]

    jobdict.update({
        # Attention: jobmode in the line below must be kept as a condor variable because it is defined dynamically in writeGriddingVARS 
        'arguments'                       : f'"-j $(jobmode) -n $(JOB) -f {parfile} -i $(this_image) -m $(msname) -c $(cfcache) -b $(libra_bundle)"',
        'notification'                    : 'Error',
        'output'                          : f'$(JOB).$(cuda_cap)x.stdout',
        'error'                           : f'$(JOB).$(cuda_cap)x.stderr',
        'log'                             : f'$(JOB).condor.log',
        'cuda_cap'                        : '$$([int(GPUs_Capability)])',
        'cuda_drv'                        : '$$([int(GPUs_DriverVersion)])',
        'request_cpus'                    : '2',
        'request_gpus'                    : '1',
        'request_memory'                  : memrequest,
        'request_disk'                    : '$(diskResidual)',
        '+UNDESIRED_Sites'                : undesired_sites,
        '+WantOSPool'                     : str(wantOSPool),
        '+DESIRED_GPU_MIN_Capability'     : min_gpu_capability,
        '+DESIRED_GPU_MIN_GlobalMemoryMb' : min_gpu_memory,
        '+DESIRED_GPU_MIN_DriverVersion'  : min_gpu_driver,
        'RequireGPUs'                     : '(Capability >= DESIRED_GPU_MIN_Capability) && (GlobalMemoryMb > DESIRED_GPU_MIN_GlobalMemoryMb) && (DriverVersion >= DESIRED_GPU_MIN_DriverVersion)',
        'transfer_input_files'            : ','.join(inputfiles),
        'transfer_output_remaps'          : '"' + ';'.join(outputremaps) + '"'
    })

    job_description = htcondor.Submit(**jobdict)
    return job_description


def createGatherJob(jobbasedict   : dict, 
                    jobmode       : str,
                    parfile       : str,
                    multiterm     : bool, 
                    imcycle       : int = 0) -> htcondor.Submit:
    
    jobdict = jobbasedict.copy()
    notgatherparameters = ['memResidual', 
                           'cfcachedir',
                           'msdir',
                           'und_sites_grid',
                           'NRPexclusive',
                           'useVLASSGPUs',
                           'casa',
                           'allspwvis']
    for key in notgatherparameters:
        if key in list(jobdict):
            del jobdict[key]

    baseimagename = jobdict.pop('baseimagename')
    workdir_0     = f'{baseimagename}.workdirectory.imcycle0'
    this_workdir  = f'{baseimagename}.workdirectory.imcycle{imcycle}'

    inputfiles = [
        '$(htcimagerfiles)',
        '$(inputfiles)',
        '$(softwaredir)/$(libra_bundle)',
        '$(softwaredir)/casa-data.tgz?pack=auto',
        f'bin/{parfile}'
    ]
    outputfiles  = ['logs.tgz']
    outputremaps = [f'logs.tgz = $(initialdir)/logs.$(JOB).tgz']

    if jobmode == 'gatherPSF':
        this_workdir = workdir_0
        useStartModel = eval(jobdict.pop('useStartModel'))
        useStartMask  = eval(jobdict.pop('useStartMask'))
        arguments = f'"-j {jobmode} -n $(JOB) -f {parfile} -i $(this_image) --gatherimagelist $(gatherimagelist) -b $(libra_bundle)"'
        imtypes = ['pb', 'psf', 'sumwt', 'weight']
        if useStartMask or useStartModel:
            outputfiles += [f'$(this_image).residual.start.tgz']
            imtypes += ['residual.start']
        if useStartMask:
            inputfiles += [f'$(imagesdir)/{baseimagename}.mask.start.tgz?pack=auto']
        if useStartModel:
            # initial multiterm model has to be converted to cube and split by spw to use here
            inputfiles  += ['$(imagesdir)/$(this_image).model.start.tgz?pack=auto']
            outputfiles += ['$(this_image).divmodel.tgz']
            #imtypes += ['divmodel']
            # following is only compatible with single-term; multi-term needs more thought
            outputremaps += [f'$(this_image).divmodel.tgz = $(imagesdir)/$(this_image).divmodel.imcycle0.tgz']
        outputfiles += [f'$(this_image).{imtype}.tgz' for imtype in imtypes]
        if multiterm:
            outputremaps += [f'$(this_image).{imtype}.tgz = $(imagesdir)/{this_workdir}/$(this_image).{imtype}.tgz' for imtype in imtypes]
        else:
            outputremaps += [f'$(this_image).{imtype}.tgz = $(imagesdir)/$(this_image).{imtype}.tgz' for imtype in imtypes]
        

    else:
        for key in ['useStartModel', 'useStartMask']:
            del jobdict[key]
        arguments = f'"-j {jobmode} -n $(JOB) -f {parfile} -i $(this_image) --gatherimagelist $(gatherimagelist) -t residual -b $(libra_bundle)"'
        # If changing imtypes, remember the '.noiter' extension in the residual remap
        imtypes = ['residual']
        outputfiles += [f'$(this_image).{imtype}.tgz' for imtype in imtypes]
        if multiterm:
            inputworkdir = f'{workdir_0}/'
            outputremaps += [f'$(this_image).{imtype}.tgz = $(imagesdir)/{this_workdir}/$(this_image).{imtype}.tgz' for imtype in imtypes]
        else:
            inputworkdir = ''
            outputremaps += [f'$(this_image).{imtype}.tgz = $(imagesdir)/$(this_image).{imtype}.imcycle{imcycle}.noiter.tgz' for imtype in imtypes]
        inputfiles += [f'$(imagesdir)/{inputworkdir}$(this_image).{imtype}.tgz?pack=auto' for imtype in ['sumwt', 'weight']]
        

    jobdict.update({
        'arguments'              : arguments,
        'notification'           : 'Error',
        'request_memory'         : '$(memModel)',
        'request_disk'           : '$(min_disk_model)',
        'output'                 : f'$(JOB).stdout',
        'error'                  : f'$(JOB).stderr',
        'log'                    : f'$(JOB).condor.log',
        'this_workdir'           : this_workdir,
        'transfer_input_files'   : ','.join(inputfiles),
        'transfer_output_files'  : ','.join(outputfiles),
        'transfer_output_remaps' : '"' + ';'.join(outputremaps) + '"'
    })

    job_description = htcondor.Submit(**jobdict)
    return job_description


def createCASAMultitermJob(jobbasedict : dict, 
                           jobmode     : str,
                           parfile     : str, 
                           nterms      : int,
                           imallkeys   : list,
                           imcycle     : int = 0) -> htcondor.Submit:
    
    htcimager_tt   = 'htcimager.py --interface casa_multiterm'
    multitermfiles = ['$(htcimagerdir)/fitsconcat.py',
                      '$(softwaredir)/$(casa).tgz?pack=auto', 
                      '$(htcimagerdir)/casa_multiterm.py']
    jobdict = jobbasedict.copy()
    notCMTJparameters = ['memResidual',
                         'cfcachedir',
                         'und_sites_grid',
                         'wantOSPool',
                         'useVLASSGPUs',
                         'NRPexclusive',
                         'useStartMask',
                         'useStartModel']
    for key in notCMTJparameters:
        if key in list(jobdict):
            del jobdict[key]
    
    baseimagename = jobdict.pop('baseimagename')
    workdir_0 = f'{baseimagename}.workdirectory.imcycle0'
    this_workdir = f'{baseimagename}.workdirectory.imcycle{imcycle}'

    inputfiles = [
        '$(htcimagerfiles)', 
        '$(multitermfiles)', 
        f'bin/{parfile}',
        '$(softwaredir)/$(libra_bundle)',
        '$(softwaredir)/casa-data.tgz?pack=auto',
        '$(msdir)/$(allspwvis).tgz?pack=auto'
    ]

    concatimagelist = ','.join(imallkeys)
    outputfiles  = ['logs.tgz']
    outputremaps = [f'logs.tgz = $(initialdir)/logs.$(JOB).tgz']

    supportImages = getSupportImageList(baseimagename, nterms)

    makeInputImageList = lambda imallkeys, imtypes: [f'$(imagesdir)/{this_workdir}/{imname}.{imtype}.tgz?pack=auto' for imtype in imtypes for imname in imallkeys]

    if jobmode == 'psfc2tt':
        inputfiles   += makeInputImageList(imallkeys, ['pb', 'psf', 'sumwt', 'weight'])
        arguments     = f'"{htcimager_tt} -j {jobmode} -n {jobmode} -f {parfile} --concatimagelist {concatimagelist} -b $(libra_bundle) --fitsconcatpath .."'
        outputfiles.append(f'{baseimagename}.weight.tgz')
        outputremaps += [f'{baseimagename}.weight.tgz = $(imagesdir)/{baseimagename}.weight.tgz']
        outputfiles  += supportImages
        outputremaps += [f'{imname} = $(imagesdir)/{imname}' for imname in supportImages]
    elif jobmode == 'resc2tt':
        arguments     = f'"{htcimager_tt} -j {jobmode} -n {jobmode} -f {parfile} --concatimagelist {concatimagelist} -t residual -b $(libra_bundle) --fitsconcatpath .."'
        inputfiles   += [f'$(imagesdir)/{imname}?pack=auto' for imname in supportImages]
        inputfiles   += makeInputImageList(imallkeys, ['residual'])
        imname        = f'{baseimagename}.residual'
        outputimages  = [f'{imname}.tgz'] + [f'{imname}.tt{N}.tgz' for N in range(nterms)]
        outputfiles  += outputimages
        outputremaps += [f'{imname}.tgz = $(imagesdir)/{imname}.imcycle{imcycle}.noiter.tgz']
        outputremaps += [f'{imname}.tt{N}.tgz = $(imagesdir)/{imname}.tt{N}.imcycle{imcycle}.noiter.tgz' for N in range(nterms)]
    elif jobmode == 'tt2cube':
        arguments     = f'"{htcimager_tt} -j {jobmode} -n {jobmode} -f {parfile} --concatimagelist {concatimagelist} -b $(libra_bundle)"'
        inputfiles   += [f'$(imagesdir)/{imname}?pack=auto' for imname in supportImages]
        inputfiles   += [f'$(imagesdir)/{baseimagename}.model.tt{N}.imcycle{imcycle}.tgz?pack=auto' for N in range(nterms)]
        inputfiles   += [f'$(imagesdir)/{workdir_0}/{imname}.{imtype}.tgz?pack=auto' for imname in imallkeys for imtype in ['sumwt', 'weight']]
        outputimages  = [f'{baseimagename}.model.tgz'] + [f'{imname}.divmodel.tgz' for imname in imallkeys]
        outputfiles  += outputimages
        outputremaps += [f'{baseimagename}.model.tgz = $(imagesdir)/{baseimagename}.model.imcycle{imcycle}.tgz']
        outputremaps += [f'{imname}.divmodel.tgz = $(imagesdir)/{this_workdir}/{imname}.divmodel.tgz' for imname in imallkeys]
    
    jobdict.update({
        'arguments'              : arguments,
        'executable'             : '$(casa)/bin/python3',
        'notification'           : 'Error',
        'request_memory'         : '$(memModel)',
        'request_disk'           : '$(min_disk_model)',
        'multitermfiles'         : ','.join(multitermfiles),
        'output'                 : f'$(JOB).stdout',
        'error'                  : f'$(JOB).stderr',
        'log'                    : f'$(JOB).condor.log',
        'transfer_input_files'   : ','.join(inputfiles),
        'transfer_output_files'  : ','.join(outputfiles),
        'transfer_output_remaps' : '"' + ';'.join(outputremaps) + '"'
    })

    job_description = htcondor.Submit(**jobdict)
    return job_description


def createUpdateModelJob(jobbasedict   : dict, 
                         jobmode       : str,
                         parfile       : str,
                         noderetry     : int,
                         imcycle       : int = 0,
                         nterms        : int = 1) -> htcondor.Submit:
    
    jobdict = jobbasedict.copy()
    baseimagename = jobdict.pop('baseimagename')

    notmodelparameters = ['memResidual',
                          'cfcachedir',
                          'msdir',
                          'und_sites_grid',
                          'wantOSPool',
                          'useVLASSGPUs',
                          'NRPexclusive',
                          'min_gpu_capability',
                          'min_gpu_memory',
                          'min_gpu_driver',
                          'casa',
                          'allspwvis']
    for key in notmodelparameters:
        if key in list(jobdict):
            del jobdict[key]

    inputfiles = [
        '$(htcimagerfiles)',
        f'bin/{parfile}',
        '$(softwaredir)/$(libra_bundle)',
        '$(softwaredir)/casa-data.tgz?pack=auto',
    ]
    outputremaps = [f'logs.tgz = $(initialdir)/logs.$(JOB).tgz']

    inputimages = []

    if noderetry == 0:
        useStartModel = eval(jobdict.pop('useStartModel'))
        useStartMask  = eval(jobdict.pop('useStartMask'))
        cycleID = 'start'
    else:
        useStartModel = False
        useStartMask  = False
        for key in ['useStartModel', 'useStartMask']:
            del jobdict[key]
        cycleID = f'imcycle{noderetry}'

    if useStartMask or noderetry > 0:
        inputimages += [f'$(imagesdir)/{baseimagename}.mask.{cycleID}.tgz?pack=auto']

    imtypes = ['pb', 'psf']

    outputremaps += [f'{baseimagename}.mask.tgz = $(imagesdir)/{baseimagename}.mask.imcycle{imcycle}.tgz']

    if jobmode == 'updateModel':
        NTerms = [1, 2 * nterms - 1]
        for imtype, numTerms in zip(imtypes, NTerms):
            for N in range(numTerms):
                inputimages.append(f'$(imagesdir)/{baseimagename}.{imtype}.tt{N}.tgz?pack=auto')
        inputimages += [f'$(imagesdir)/{baseimagename}.residual.tt{N}.imcycle{noderetry}.noiter.tgz?pack=auto' for N in range(nterms)]
        if useStartModel or noderetry > 0:
            inputimages += [f'$(imagesdir)/{baseimagename}.model.tt{N}.{cycleID}.tgz?pack=auto' for N in range(nterms)]
        for imtype in ['residual', 'model']:
            for N in range(nterms):
                imname = f'{baseimagename}.{imtype}.tt{N}'
                outputremaps += [f'{imname}.tgz = $(imagesdir)/{imname}.imcycle{imcycle}.tgz']
    elif jobmode == 'runModelCycle':
        imtypes += ['sumwt', 'weight']
        inputimages  = [f'$(imagesdir)/{baseimagename}.{imtype}.tgz?pack=auto' for imtype in imtypes]
        inputimages += [f'$(imagesdir)/{baseimagename}.residual.imcycle{noderetry}.noiter.tgz?pack=auto']
        if (useStartModel) or noderetry > 0:
            inputimages += [f'$(imagesdir)/{baseimagename}.model.{cycleID}.tgz?pack=auto']
        for imtype in ['residual', 'model', 'divmodel']:
            imname = f'{baseimagename}.{imtype}'
            outputremaps += [f'{imname}.tgz = $(imagesdir)/{imname}.imcycle{imcycle}.tgz']

    inputfiles += inputimages

    jobdict.update({
        'arguments'              : f'"-j {jobmode} -n {jobmode} -f {parfile} -i {baseimagename} -b $(libra_bundle)"',
        'notification'           : 'Always',
        'request_memory'         : '$(memModel)',
        'request_disk'           : '$(min_disk_model)',
        'output'                 : f'$(JOB).stdout',
        'error'                  : f'$(JOB).stderr',
        'log'                    : f'$(JOB).condor.log',
        'transfer_input_files'   : ','.join(inputfiles),
        'transfer_output_remaps' : '"' + ';'.join(outputremaps) + '"'
    })

    job_description = htcondor.Submit(**jobdict)
    return job_description


def createRestoreJob(jobbasedict   : dict, 
                     jobmode       : str,
                     parfile       : str,
                     multiterm     : bool,
                     imcycle       : int = 0,
                     nterms        : int = 1) -> htcondor.Submit:
    
    jobdict = jobbasedict.copy()
    baseimagename = jobdict.pop('baseimagename')

    notrestoreparameters = ['memResidual',
                            'cfcachedir',
                            'msdir',
                            'und_sites_grid',
                            'wantOSPool',
                            'useVLASSGPUs',
                            'NRPexclusive',
                            'min_gpu_capability',
                            'min_gpu_memory',
                            'min_gpu_driver',
                            'casa',
                            'useStartModel',
                            'useStartMask',
                            'allspwvis']
    for key in notrestoreparameters:
        if key in list(jobdict):
            del jobdict[key]

    inputfiles = [
        '$(htcimagerfiles)',
        f'bin/{parfile}',
        '$(softwaredir)/$(libra_bundle)',
        '$(softwaredir)/casa-data.tgz?pack=auto',
    ]
    outputremaps = [f'logs.tgz = $(initialdir)/logs.$(JOB).tgz']
    outimtypes   = ['image', 'image.pbcor']
    inputimages  = [f'$(imagesdir)/{baseimagename}.mask.imcycle{imcycle}.tgz?pack=auto']

    if multiterm:
        imtypes = ['pb', 'psf', 'sumwt']
        NTerms  = [1, 2 * nterms - 1, 2 * nterms - 1]
        for imtype, numTerms in zip(imtypes, NTerms):
            for N in range(numTerms):
                inputimages.append(f'$(imagesdir)/{baseimagename}.{imtype}.tt{N}.tgz?pack=auto')
        imtypes = ['model', 'residual']
        NTerms  = [nterms, nterms]
        for imtype, numTerms in zip(imtypes, NTerms):
            for N in range(numTerms):
                inputimages.append(f'$(imagesdir)/{baseimagename}.{imtype}.tt{N}.imcycle{imcycle}.tgz?pack=auto')
        for imtype in outimtypes:
            for N in range(nterms):
                imname = f'{baseimagename}.{imtype}.tt{N}'
                outputremaps.append(f'{imname}.tgz = $(imagesdir)/{imname}.tgz')
    else:
        imtypes = ['pb', 'psf', 'sumwt', 'weight', f'residual.imcycle{imcycle}', f'model.imcycle{imcycle}']
        inputimages += [f'$(imagesdir)/{baseimagename}.{imtype}.tgz?pack=auto' for imtype in imtypes]
        for imtype in outimtypes:
            imname = f'{baseimagename}.{imtype}'
            outputremaps.append(f'{imname}.tgz = $(imagesdir)/{imname}.tgz')

    inputfiles += inputimages

    jobdict.update({
        'arguments'              : f'"-j {jobmode} -n {jobmode} -f {parfile} -i {baseimagename} -b $(libra_bundle)"',
        'notification'           : 'Always',
        'request_memory'         : '$(memModel)',
        'request_disk'           : '$(min_disk_model)',
        'output'                 : f'$(JOB).stdout',
        'error'                  : f'$(JOB).stderr',
        'log'                    : f'$(JOB).condor.log',
        'transfer_input_files'   : ','.join(inputfiles),
        'transfer_output_remaps' : '"' + ';'.join(outputremaps) + '"'
    })
    
    job_description = htcondor.Submit(**jobdict)
    return job_description
