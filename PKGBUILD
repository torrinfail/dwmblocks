pkgname=dwmblocks
pkgver=0
pkgrel=0
pkgdesc="Modular status bar for dwm written in c."
arch=('any')
license=('GPL2')

package() {
	cd ..
	make DESTDIR="$pkgdir" install
}
