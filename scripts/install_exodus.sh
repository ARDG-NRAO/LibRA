# $1 should be an absolute path.  Or else pip gets very upset!
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <INSTALL-PATH>"
    echo "Note: INSTALL-PATH should be an absolute path.  Or else pip gets very upset!"
    exit 0;
fi
git clone https://github.com/sanbee/exodus_sbfork.git
export PYTHONUSERBASE=$1 #$PWD/exodus
pip3 install --user exodus_sbfork/
