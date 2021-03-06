import os
import instinctual
from instinctual.informer import getServerRoot

conf = instinctual.getConf()

root = os.sep.join(os.path.abspath(__file__).split(os.sep)[:-3])
media_path = os.sep.join([root, 'media'])

# set the umask
os.umask(0000)

# Django settings for instinctual project.
DEBUG = instinctual.isDebug()
TEMPLATE_DEBUG = DEBUG

ADMINS = (
    ('informer', 'support@informerapp.com'),
)

MANAGERS = ADMINS

db_engine = conf.get('informer', 'db_engine')
db_name = conf.get('informer', 'db_name')
db_user = conf.get('informer', 'db_user')
db_pass = conf.get('informer', 'db_pass')
db_host = conf.get('informer', 'db_host')
db_port = conf.get('informer', 'db_port')

DATABASE_ENGINE   = db_engine   # 'postgresql_psycopg2', 'postgresql', 'mysql', 'sqlite3' or 'ado_mssql'.
DATABASE_NAME     = db_name     # Or path to database file if using sqlite3.
DATABASE_USER     = db_user     # Not used with sqlite3.
DATABASE_PASSWORD = db_pass     # Not used with sqlite3.
DATABASE_HOST     = db_host     # Set to empty string for localhost. Not used with sqlite3.
DATABASE_PORT     = db_port     # Set to empty string for default. Not used with sqlite3.

# Local time zone for this installation. Choices can be found here:
# http://www.postgresql.org/docs/8.1/static/datetime-keywords.html#DATETIME-TIMEZONE-SET-TABLE
# although not all variations may be possible on all operating systems.
# If running in a Windows environment this must be set to the same as your
# system time zone.
TIME_ZONE = 'America/Los_Angeles'

# Language code for this installation. All choices can be found here:
# http://www.w3.org/TR/REC-html40/struct/dirlang.html#langcodes
# http://blogs.law.harvard.edu/tech/stories/storyReader$15
LANGUAGE_CODE = 'en-us'

SITE_ID = 1

# If you set this to False, Django will make some optimizations so as not
# to load the internationalization machinery.
USE_I18N = True

# Absolute path to the directory that holds media.
# Example: "/home/media/media.lawrence.com/"
MEDIA_ROOT = media_path

# URL that handles the media served from MEDIA_ROOT.
# Example: "http://media.lawrence.com"
MEDIA_URL = getServerRoot() + '/media/'

# URL prefix for admin media -- CSS, JavaScript and images. Make sure to use a
# trailing slash.
# Examples: "http://foo.com/media/", "/media/".
ADMIN_MEDIA_PREFIX = '/media/admin/'

# Make this unique, and don't share it with anybody.
SECRET_KEY = 'gq#gb$_7cm=+fk*$iqa9&)LUNAROCKS#v(#&7^g*tpi1+li!j5b'

# List of callables that know how to import templates from various sources.
TEMPLATE_LOADERS = (
    'django.template.loaders.filesystem.load_template_source',
    'django.template.loaders.app_directories.load_template_source',
#     'django.template.loaders.eggs.load_template_source',
)

MIDDLEWARE_CLASSES = (
    'django.middleware.common.CommonMiddleware',
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.middleware.doc.XViewMiddleware',
    'django.middleware.transaction.TransactionMiddleware',
)

ROOT_URLCONF = 'instinctual.urls'

TEMPLATE_DIRS = (
    # Put strings here, like "/home/html/django_templates" or "C:/www/django/templates".
    # Always use forward slashes, even on Windows.
    # Don't forget to use absolute paths, not relative paths.
    os.sep.join([root, 'templates']),
    os.sep.join([root, 'templates', 'html']),
)

INSTALLED_APPS = (
    'instinctual.informer',
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.sites',
)

SERIALIZATION_MODULES = {
    'custom_xml' : 'instinctual.serializers.custom_xml',
    'custom_json' : 'instinctual.serializers.custom_json',
}
