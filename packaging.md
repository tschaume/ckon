```
# http://anonscm.debian.org/cgit/debian-science/packages/ckon.git/
cd /vagrant/others
wget https://github.com/tschaume/ckon/archive/v<version>.tar.gz
mv v<version>.tar.gz ckon-<version>.orig.tar.gz
cd others
git import-orig --pristine-tar ../ckon-<version>.orig.tar.gz
# update changelog (unstable, correct version)
git-dch -S -a
# update version in ckon.1 manual
git commit -a -m "update version and changelog"
git clean -f -x -d
gbp buildpackage --git-pristine-tar
cd ..
sudo debi ckon_<version>-<rev>_amd64.changes
ckon --version
lintian -i -I --show-overrides ckon_<version>-<rev>_amd64.changes
cd others
# fix errors
gbp buildpackage --git-pristine-tar
cme fix dpkg-control
gbp buildpackage --git-pristine-tar --git-tag
git push --all alioth
git push --tags alioth
git push --all origin
git push --tags origin
cd ..
dput mentors-ftp ckon_<version>-<rev>_amd64.changes
# https://mentors.debian.net/packages/my
# https://github.com/tschaume/ckon-deb-pkg/releases
# http://anonscm.debian.org/cgit/blends/projects/science.git/ -> Suggests: ckon
```
