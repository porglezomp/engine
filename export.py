import bpy
import bmesh
import sys


def export_model(bm, out_file):
    print('vert', 6, len(bm.verts), file=out_file)
    for v in bm.verts:
        x, y, z = v.co
        r, s, t = v.normal
        print(x, y, z, r, s, t, file=out_file)
    print('index', len(bm.faces), file=out_file)
    for f in bm.faces:
        print(' '.join(str(v.index) for v in f.verts), file=out_file)


def main(fname):
    # Merge all the meshes into one bmesh
    bm = bmesh.new()
    for obj in bpy.data.objects:
        if obj.type != 'MESH':
            continue
        bm.from_mesh(obj.to_mesh(
            bpy.context.scene,
            apply_modifiers=True,
            settings='RENDER'
        ))

    # Triangulate
    bmesh.ops.triangulate(bm, faces=bm.faces)

    print("Exporting file to {}...".format(fname))

    with open(fname, 'w') as out_file:
        export_model(bm, out_file)

    print("Object exported.")

    # Cleanup
    bm.free()


if __name__ == '__main__':
    delim = sys.argv.index('--')
    args = sys.argv[delim+1:]
    if not args:
        print("Error! Pass an output file name")
        sys.exit(1)
    main(args[0])
    sys.exit(0)
