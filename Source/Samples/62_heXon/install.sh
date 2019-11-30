#!/bin/sh

./.installreq.sh
./.buildurho.sh

git pull
cd ..
mkdir heXon-build
cd heXon-build
qmake ../heXon/heXon.pro
sudo make install
sudo chown -R $USER ~/.local/share/luckey/hexon/
sudo chown $USER ~/.local/share/icons/hexon.svg
update-icon-caches ~/.local/share/icons/
cd ..
rm -rf heXon-build
