# $1 should be an absolute path.  Or else pip gets very upset!
git clone https://github.com/sanbee/exodus_sbfork.git
export PYTHONUSERBASE=$1 #$PWD/exodus
pip3 install --user exodus_sbfork/
