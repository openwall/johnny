TEMPLATE = subdirs
SUBDIRS = johnny
win32: SUBDIRS +=helper

johnny.file = johnny.pro
helper.file = helper/helper.pro
