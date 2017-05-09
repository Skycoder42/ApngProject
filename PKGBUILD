# Maintainer: Felix Barz <skycoder42.de@gmx.de>
pkgname=apng-project
pkgver=2.0.0
pkgrel=1
pkgdesc="A Qt image plugin for APNG files + Video to APNG converter tool"
arch=('i686' 'x86_64')
url="https://github.com/Skycoder42/ApngProject"
license=('BSD')
depends=('qt5-base' 'qt5-tools' 'libpng' 'apng-utils')
makedepends=('git' 'qpm')
source=("$pkgname-$pkgver::git+https://github.com/Skycoder42/ApngProject.git") #tag=$pkgver
md5sums=('SKIP')

build() {
  cd "$pkgname-$pkgver"

  cd VideoImageConverter
  qpm install
  cd ..

  qmake
  make qmake_all
  make
}

package() {
  cd "$pkgname-$pkgver"
  
  make INSTALL_ROOT="$pkgdir" install
  
  install -D -m644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}