use std;
use std::path::{Path, PathBuf};
use std::io::Read;

use glium::{Display, Program};

use asset::AssetError;


pub fn load_shader<P: AsRef<Path>>(display: &Display, name: P) -> Result<Program, AssetError> {
    let mut path_buf = PathBuf::from(name.as_ref());

    // Load the vertex shader source
    path_buf.set_extension("vert");
    let mut vert_file = std::fs::File::open(&path_buf).map_err(|e| {
        AssetError::FileError(path_buf.clone(), e)
    })?;
    let mut vert_src = String::new();
    vert_file.read_to_string(&mut vert_src).map_err(|e| {
        AssetError::FileError(path_buf.clone(), e)
    })?;

    // Load the fragment shader source
    path_buf.set_extension("frag");
    let mut frag_file = std::fs::File::open(&path_buf).map_err(|e| {
        AssetError::FileError(path_buf.clone(), e)
    })?;
    let mut frag_src = String::new();
    frag_file.read_to_string(&mut frag_src).map_err(|e| {
        AssetError::FileError(path_buf.clone(), e)
    })?;

    // @Todo: Attempt to load geometry/tesselation/etc. shaders
    Ok(
        Program::from_source(display, &vert_src, &frag_src, None)
            .map_err(|e| AssetError::ShaderError(PathBuf::from(name.as_ref()), e))?,
    )
}
