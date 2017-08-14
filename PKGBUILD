# Maintainer: Felix Barz <skycoder42.de@gmx.de>
pkgname=apng-project
pkgver=2.0.1
pkgrel=1
pkgdesc="Video to APNG converter tool"
arch=('i686' 'x86_64')
url="https://github.com/Skycoder42/ApngProject"
license=('BSD')
depends=('qt5-base' 'qt5-multimedia' 'apng-utils')
makedepends=('git' 'qpm' 'qt5-tools')
_pkgfqn=$pkgname-$pkgver
source=("$_pkgfqn::git+https://github.com/Skycoder42/ApngProject.git") #tag=$pkgver
md5sums=('SKIP')

prepare() {
  mkdir -p build
  
  cd "$_pkgfqn"
  qpm install
}

build() {
  cd build

  qmake -r "../$_pkgfqn/"
  make
}

package() {
  cd build
  make INSTALL_ROOT="$pkgdir" install

  cd "../$_pkgfqn/"
  install -D -m644 videoimageconverter.desktop "$pkgdir/usr/share/applications/videoimageconverter.desktop"
  install -D -m644 apng-vic_32.png "$pkgdir/usr/share/icons/hicolor/32x32/apps/apng-vic.png"
  install -D -m644 apng-vic_16.png "$pkgdir/usr/share/icons/hicolor/16x16/apps/apng-vic.png"
  install -D -m644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
