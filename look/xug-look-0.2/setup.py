from distutils.core import setup
from DistUtilsExtra.command.build_extra import build_extra
from DistUtilsExtra.command.build_i18n import build_i18n
import glob


setup(
    name = 'xug-look',
    version = '0.1',
    data_files=[
		('share/backgrounds/', glob.glob('xug-wallpapers/*')), 
                ('lib/plymouth/themes/xug-logo', glob.glob('lib/plymouth/themes/xug-logo/*')),
		],
    cmdclass = { "build" : build_extra,
                 "build_i18n" :  build_i18n }
)

