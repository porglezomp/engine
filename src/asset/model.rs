use std;
use std::path::{Path, PathBuf};

use glium;

use super::AssetError;

#[derive(Copy, Clone, Debug)]
pub struct Vert {
    pos: [f32; 3],
    norm: [f32; 3],
}
implement_vertex!(Vert, pos, norm);

#[derive(Debug)]
struct ModelData {
    vertex: Vec<Vert>,
    index: Vec<u32>,
}

#[derive(Debug)]
pub(super) struct ModelResource {
    data: ModelData,
    pub vertex: glium::VertexBuffer<Vert>,
    pub index: glium::IndexBuffer<u32>,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub struct ModelHandle(u32);

impl ModelHandle {
    pub fn raw_id(&self) -> u32 {
        self.0
    }

    pub unsafe fn from_raw_parts(id: u32) -> ModelHandle {
        ModelHandle(id)
    }
}

impl ModelData {
    fn load<R: std::io::BufRead>(input: &mut R) -> Result<ModelData, ModelError> {
        let mut buf = String::new();
        input.read_line(&mut buf).map_err(|e| ModelError::Io(1, e))?;
        let vert_count = {
            let parts: Vec<_> = buf.trim().split_whitespace().collect();
            if parts.len() != 3 || parts[0] != "vert" {
                return Err(ModelError::MissingVertCount(1));
            }
            let component_count: usize = parts[1].parse().map_err(|e| ModelError::ParseInt(1, e))?;
            if component_count != 6 {
                return Err(ModelError::BadCount(component_count));
            }
            parts[2].parse().map_err(|e| ModelError::ParseInt(1, e))?
        };

        let mut vertex = Vec::with_capacity(vert_count);
        for line in (0..vert_count).map(|x| x + 2) {
            buf.clear();
            input.read_line(&mut buf).map_err(
                |e| ModelError::Io(line, e),
            )?;
            let parts = buf.trim()
                .split_whitespace()
                .map(|x| x.parse())
                .collect::<Result<Vec<_>, _>>()
                .map_err(|e| ModelError::ParseFloat(line, e))?;

            vertex.push(Vert {
                pos: [parts[0], parts[1], parts[2]],
                norm: [parts[3], parts[4], parts[5]],
            });
        }

        let idx_line = 2 + vert_count;
        buf.clear();
        input.read_line(&mut buf).map_err(
            |e| ModelError::Io(idx_line, e),
        )?;
        let index_count: usize = {
            let parts: Vec<_> = buf.trim().split_whitespace().collect();
            if parts.len() != 2 || parts[0] != "index" {
                return Err(ModelError::MissingIndexCount(idx_line));
            }
            parts[1].parse().map_err(
                |e| ModelError::ParseInt(idx_line, e),
            )?
        };

        let mut index = Vec::with_capacity(index_count);
        for line in (0..index_count).map(|x| x + idx_line) {
            buf.clear();
            input.read_line(&mut buf).map_err(
                |e| ModelError::Io(line, e),
            )?;
            for item in buf.trim().split_whitespace().map(|x| x.parse()) {
                index.push(item.map_err(|e| ModelError::ParseInt(line, e))?)
            }
        }

        Ok(ModelData { vertex, index })
    }
}

pub(super) fn load_model<P: AsRef<Path>>(
    display: &glium::Display,
    name: P,
) -> Result<ModelResource, AssetError> {
    let mut path_buf = PathBuf::from(name.as_ref());
    path_buf.set_extension("model");
    // @Error
    let file = std::fs::File::open(&path_buf).unwrap();
    let mut buf_read = std::io::BufReader::new(file);
    // @Error
    let model_data = ModelData::load(&mut buf_read).expect("Should load");
    // @Error
    let vertex_buffer = glium::VertexBuffer::new(display, &model_data.vertex).unwrap();
    // @Error
    let index_buffer = glium::IndexBuffer::new(
        display,
        glium::index::PrimitiveType::TrianglesList,
        &model_data.index,
    ).unwrap();

    Ok(ModelResource {
        data: model_data,
        vertex: vertex_buffer,
        index: index_buffer,
    })
}

impl Into<::host::ModelId> for ModelHandle {
    fn into(self) -> ::host::ModelId {
        ::host::ModelId(self.0)
    }
}

impl From<::host::ModelId> for ModelHandle {
    fn from(id: ::host::ModelId) -> ModelHandle {
        ModelHandle(id.0)
    }
}

#[derive(Debug)]
pub enum ModelError {
    Io(usize, std::io::Error),
    ParseInt(usize, std::num::ParseIntError),
    ParseFloat(usize, std::num::ParseFloatError),
    BadCount(usize),
    MissingVertCount(usize),
    MissingIndexCount(usize),
}
