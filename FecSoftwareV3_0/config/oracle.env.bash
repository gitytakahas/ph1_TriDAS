ENV_CMS_TK_ORACLE_VERSION=10203
# By default check for a default client in /exports/xdaq/CMSSW/Local/slc3_ia32_gcc323/external/oracle/10.2.0.2/
# If any parameter is specified then try to detect the different possibilitites
if [ -z "$1" ]; then
  useClient="local"
else
  useClient=$1
fi

# Use the local client
CMSSWCLIENT=/opt/deps/slc4_ia32_gcc345/external/oracle/10.2.0.2/
CMSSWTNSADMIN=/opt/deps/slc4_ia32_gcc345/cms/oracle-env/1.4/etc/
if [ $useClient = local ]
then
  if test -d $CMSSWCLIENT
  then
      export ENV_ORACLE_HOME=$CMSSWCLIENT
      export TNS_ADMIN=$CMSSWTNSADMIN
      if [ -f $TNS_ADMIN/tnsnames.ora ];
      then
        echo "Use client in $ENV_ORACLE_HOME"
      else
        echo "Error: did not find the tnsnames.ora file for the definition of DB access"
      fi
  else
    useClient=afs
  fi
fi

# Try to find a AFS client
if [ $useClient = afs ]; then
  if test -d /afs/cern.ch/project/oracle/@sys/$ENV_CMS_TK_ORACLE_VERSION
  then
    source /afs/cern.ch/project/oracle/script/setoraenv.sh -s $ENV_CMS_TK_ORACLE_VERSION
    export ENV_ORACLE_HOME=$ORACLE_HOME
    echo "The database can be used via AFS (version = $ENV_CMS_TK_ORACLE_VERSION)"
  else
    if [ -n "$ENV_ORACLE_HOME" ]
    then
      export TNS_ADMIN=$ENV_ORACLE_HOME/../../../network/admin
      export LD_LIBRARY_PATH=$ENV_ORACLE_HOME/lib:${LD_LIBRARY_PATH}
      export PATH=$ENV_ORACLE_HOME/bin:${PATH}
      echo "The database can be used via deployment kit set in ${ENV_ORACLE_HOME}"
    fi
  fi
fi

# Error ? 
if test -z $TNS_ADMIN
then
        echo "No database can be used"
fi

