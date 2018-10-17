import bpy
import bmesh
import sys
import traceback
import mathutils

from bpy_extras.io_utils import create_derived_objects, free_derived_objects

argv = sys.argv
argv = argv[argv.index("--") + 1:]

outputPath = argv[0]

def run():
  file = open(outputPath, 'w')
  file.write("# id x y z \n")
  for obj in bpy.data.objects:
    if obj.name.startswith("f."):
      name = obj.name[2:]
      line = name
      line += " "
      line += str(round(obj.location.x,4))
      line += " "
      line += str(round(obj.location.y,4))
      line += " "
      line += str(round(obj.location.z,4))
      # line += " "
      # line += str(obj.rotation_euler)
      file.write(line + "\n");
      bpy.data.objects.remove(obj)
  file.close()

try:
  run()
  sys.exit(0)
except Exception:
  traceback.print_exc()
  sys.exit(1)


