import sys
import os

try:
	ToolkitDir = os.environ['VCToolkitInstallDir']
	PlatSdkDir = os.environ['PlatformSDKInstallDir']
except:
	print "Please set environment variable VCToolkitInstallDir to point to your VC2003TK installation folder"
	sys.exit(-1)

ToolkitBinDir = os.path.join(ToolkitDir, 'bin')
ToolkitIncDir = os.path.join(ToolkitDir, 'include')
ToolkitLibDir = os.path.join(ToolkitDir, 'lib')

PlatSdkBinDir = os.path.join(PlatSdkDir, 'bin')
PlatSdkIncDir = os.path.join(PlatSdkDir, 'include')
PlatSdkLibDir = os.path.join(PlatSdkDir, 'lib')

ToolkitCc = os.path.join(ToolkitBinDir, 'cl.exe')
ToolkitLink = os.path.join(ToolkitBinDir, 'link.exe')

mymode = ARGUMENTS.get('mode', 'release') 
if not (mymode in ['debug', 'release']):
	print "Error: expected 'debug' or 'release', found: " + mymode
	Exit(1)

if mymode == 'release':
	cflags = '/nologo /c /EHsc /O1isb2 /GTAL /GF /MT /W4 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"'
	linkflags = '/nologo /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /STACK:1048576 /OPT:REF /OPT:ICF /FILEALIGN:512 /LTCG /MACHINE:X86 /RELEASE'
elif mymode == 'debug':
	cflags = '/blub'
	linkflags = '/blub'

env = Environment(
	CC = ToolkitCc,
	CPPPATH = ToolkitIncDir,
	CCFLAGS = cflags,
	LINK = ToolkitLink,
	LINKFLAGS = linkflags,
	LIBPATH = ToolkitLibDir
)

env['ENV']['INCLUDE'] = [ToolkitIncDir, PlatSdkIncDir]
env['ENV']['LIB'] = [ToolkitLibDir, PlatSdkLibDir]

Export('env', 'mymode')

SConscript('src/SConscript', build_dir=os.path.join('build', mymode), duplicate=0)
