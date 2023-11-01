#!/bin/bash

param=$1

# Get the hint from an environment variable. This is used for detached head builds
# Default grep is master
headGrep="\\\-mas-"
tagid="mas"
#echo $CASABRANCHHINT
if [ ! -z $CASABRANCHHINT ]; then
    if [[ $CASABRANCHHINT =~ ^feature.*CAS.* ]] ; then
        b1=${CASABRANCHHINT%/*} # part before the slash
        b2=${CASABRANCHHINT##*/} # part after the slash
        headGrep=$b1-$b2
    elif [[ $CASABRANCHHINT =~ ^bugfix.*CAS.* ]] ; then
        b1=${CASABRANCHHINT%/*} # part before the slash
        b2=${CASABRANCHHINT##*/} # part after the slash
        headGrep=$b1-$b2
    elif [[ $CASABRANCHHINT =~ ^CAS.* ]] ; then
        headGrep=$CASABRANCHHINT
    elif [[ $CASABRANCHHINT =~ ^ARD.* ]] ; then
        headGrep=$CASABRANCHHINT
    elif [[ $CASABRANCHHINT =~ .*release.* ]] ; then
        headGrep=$CASABRANCHHINT
        tagid="rel"
    elif [[ $CASABRANCHHINT =~ ^bambooprtest.* ]] ; then
        headGrep="bambooprtest"
    fi
fi
#echo $headGrep

# Check where the current "HEAD" points to.
branch=`git rev-parse --abbrev-ref HEAD`

# Detached HEAD, should have a tag
if [ $branch == "HEAD" ];then
    headTag=`git tag --points-at HEAD | grep $headGrep`
    if [[ -z "${headTag// }" ]]; then
        # Get the nearest tag and add Desc
        headCommit=`git rev-parse HEAD`
        headTag=`git tag --points-at HEAD | grep $headGrep | xargs`
    fi
    CASA_VERSION_DESC=$headTag
    # $CASAFORKPOINTHINT is the fork point commit
    # You can obtain this by executing  "git merge-base --fork-point master"
    # while in the branch, but before detaching the HEAD
    if [ -z $CASAFORKPOINTHINT ]; then
        if [[ $CASABRANCHHINT =~ .*release.* ]]; then
            CASAFORKPOINTHINT=`git merge-base $CASABRANCHHINT $branch`
        else
            CASAFORKPOINTHINT=`git merge-base master $branch`
        fi
    fi
    if [ $tagid == "rel" ]; then
        headTag=`git describe --abbrev=0 --tags --match='[0-9]*.[0-9]*.[0-9]*-rel-[0-9]*' $(git rev-parse $CASAFORKPOINTHINT)`
    else
        headTag=`git describe --abbrev=0 --tags --match='[0-9]*.[0-9]*.[0-9]*-mas-[0-9]*' $(git rev-parse $CASAFORKPOINTHINT)`
    fi
    #echo "${headTag##*-};$CASA_VERSION_DESC"
    # --curr #echo "${headTag##*-}; "
    if [ "$param" == "--pretty-print" ];then
        if [ -n "$CASA_VERSION_DESC" ] && [[ "$CASA_VERSION_DESC" != *"-$tagid-"* ]]; then
            #echo "${headTag%-mas*}-${headTag##*-}+"
            echo "${headTag}+"
        else
            echo "${headTag}"
            #echo "${headTag%-mas*}-${headTag##*-}"
        fi
    else
            # Don't include version desc for master
            if [[ $CASA_VERSION_DESC == *"-$tagid-"* ]];then
                echo "${headTag##*-};"
            else
                echo "${headTag##*-};$CASA_VERSION_DESC"
            fi
    fi
# Master
elif [ $branch == "master" ];then
    #echo "Resolving master"
    masterTag=`git tag --points-at HEAD | grep \\\-mas- | xargs`
    if [[ -z "${masterTag// }" ]]; then
        # Get the nearest tag and add Desc
        headCommit=`git rev-parse HEAD`
        masterTag=`git describe --abbrev=0 --match='[0-9]*.[0-9]*.[0-9]*-mas-[0-9]*' $(git rev-parse $CASAFORKPOINTHINT)`
        CASA_VERSION_DESC="ID $headCommit "
    fi
    if [ "$param" == "--pretty-print" ];then
        if [ -n "$CASA_VERSION_DESC" ] && [[ "$CASA_VERSION_DESC" != *"-mas-"* ]]; then
            #echo "${masterTag%-mas*}-${masterTag##*-}+"
            echo "${masterTag}+"
        else
            #echo "${masterTag%-mas*}-${masterTag##*-}"
            echo "${masterTag}"
        fi
    else
        # Return only the revision number
        echo "${masterTag##*-};$CASA_VERSION_DESC"
    fi
elif [[ $branch =~ ^release\/.* ]];then
    #echo "Resolving release"
    masterTag=`git tag --points-at HEAD | grep \\\-rel- | xargs`
    if [[ -z "${masterTag// }" ]]; then
        # Get the nearest tag and add Desc
        headCommit=`git rev-parse HEAD`
        masterTag=`git describe --abbrev=0 --match='[0-9]*.[0-9]*.[0-9]*-rel-[0-9]*' $(git rev-parse $CASAFORKPOINTHINT)`
        #CASA_VERSION_DESC="ID $headCommit "
    fi
    # Return only the revision number
    echo "${masterTag##*-};$CASA_VERSION_DESC"
# Any other branch
else
    #echo "Resolving branch"
    # Using parameter expansion to split the strings
    # Replace slash in branch name with dash for tags
    if [[ $string == *"/"* ]]; then
        b1=${branch%/*} # part before the slash
        b2=${branch##*/} # rpart after the slash
        tagMatcher=$b1-$b2
        #echo "b1: $b1"
        #echo "b2: $b2"
    # If there is no slash use the branch name as is
    else
        tagMatcher=$branch
        #echo $tagMatcher
    fi
    branchTag=`git tag --points-at HEAD | grep \\\-$tagMatcher- | xargs`
    if [[ -z "${branchTag// }" ]]; then
        # Get the nearest tag and add Desca
        headCommit=`git rev-parse HEAD`
        branchTag=`git describe --abbrev=0 | grep \\\-$tagMatcher- | xargs`
        CASA_VERSION_DESC="ID $headCommit "
    else
        CASA_VERSION_DESC="BT $branchTag"
    fi
    # Do our best to resolve the master tag for revision even when we have
    # Branch tag
    if [ -z $CASAFORKPOINTHINT ]; then
        CASAFORKPOINTHINT=`git merge-base master $branch`
    fi
    masterTag=`git describe --abbrev=0 --match='[0-9]*.[0-9]*.[0-9]*-mas-[0-9]*' $(git rev-parse $CASAFORKPOINTHINT)`
    #masterTag=`git describe --tags $(git rev-parse $CASAFORKPOINTHINT)`
    if [ "$param" == "--pretty-print" ];then
        if [ -n CASA_VERSION_DESC ]; then
            #echo "${masterTag%-mas*}-${masterTag##*-}+"
            echo "${masterTag}+"
        else
            echo "${masterTag%-mas*}-${masterTag##*-}"
            echo "${masterTag}"
        fi
    else
        echo "${masterTag##*-};$CASA_VERSION_DESC"
    fi
    # Remove the feature/release information
    # and return a simple version number
    #mTag=${branchTag%-feature-*}
    #mTag=${branchTag%-release-*}
    #echo $mTag
    # Return the master tag version
    #echo "${mTag##*-};$CASA_VERSION_DESC"
fi
