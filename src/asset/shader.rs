use std;
use std::path::{Path, PathBuf};
use std::io::Read;

use glium::{Display, Program};

use asset::AssetError;

pub(super) struct ShaderResource {
    pub(super) program: Program,
}

#[derive(Copy, Clone, PartialEq, Eq, Debug, Hash)]
pub struct ShaderHandle(u32);

impl ShaderHandle {
    pub fn raw_id(&self) -> u32 {
        self.0
    }

    pub unsafe fn from_raw_parts(id: u32) -> ShaderHandle {
        ShaderHandle(id)
    }
}

pub(super) fn load_shader<P: AsRef<Path>>(
    display: &Display,
    name: P,
) -> Result<ShaderResource, AssetError> {
    let mut path_buf = PathBuf::from(name.as_ref());

    // Load the vertex shader source
    path_buf.set_extension("vert");
    let mut vert_file = std::fs::File::open(&path_buf).map_err(|e| {
        AssetError::File(path_buf.clone(), e)
    })?;
    let mut vert_src = String::new();
    vert_file.read_to_string(&mut vert_src).map_err(|e| {
        AssetError::File(path_buf.clone(), e)
    })?;

    // Load the fragment shader source
    path_buf.set_extension("frag");
    let mut frag_file = std::fs::File::open(&path_buf).map_err(|e| {
        AssetError::File(path_buf.clone(), e)
    })?;
    let mut frag_src = String::new();
    frag_file.read_to_string(&mut frag_src).map_err(|e| {
        AssetError::File(path_buf.clone(), e)
    })?;

    // @Todo: Attempt to load geometry/tesselation/etc. shaders
    Ok(ShaderResource {
        program: Program::from_source(display, &vert_src, &frag_src, None)
            .map_err(|e| AssetError::Shader(PathBuf::from(name.as_ref()), e))?,
    })
}

impl Into<::host::ShaderId> for ShaderHandle {
    fn into(self) -> ::host::ShaderId {
        ::host::ShaderId(self.0)
    }
}
