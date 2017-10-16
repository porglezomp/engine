use std::io;
use std::path::PathBuf;

use glium;

pub mod shader;
pub mod model;

#[derive(Debug)]
pub enum AssetError {
    FileError(PathBuf, io::Error),
    ShaderError(PathBuf, glium::ProgramCreationError),
}
