use std;
use glium;

#[derive(Copy, Clone, Debug)]
pub struct Vert {
    pos: [f32; 3],
    norm: [f32; 3],
}
implement_vertex!(Vert, pos, norm);

#[derive(Debug)]
pub struct ModelData {
    pub vertex: Vec<Vert>,
    pub index: Vec<u16>,
}

impl ModelData {
    pub fn load<R: std::io::BufRead>(input: &mut R) -> Result<ModelData, Error> {
        let mut buf = String::new();
        input.read_line(&mut buf).map_err(|e| Error::Io(1, e))?;
        let vert_count = {
            let parts: Vec<_> = buf.trim().split_whitespace().collect();
            if parts.len() != 3 || parts[0] != "vert" {
                return Err(Error::MissingVertCount(1));
            }
            let component_count: usize = parts[1].parse().map_err(|e| Error::ParseInt(1, e))?;
            if component_count != 6 {
                return Err(Error::BadCount(component_count));
            }
            parts[2].parse().map_err(|e| Error::ParseInt(1, e))?
        };

        let mut vertex = Vec::with_capacity(vert_count);
        for line in (0..vert_count).map(|x| x + 2) {
            buf.clear();
            input.read_line(&mut buf).map_err(|e| Error::Io(line, e))?;
            let parts = buf.trim()
                .split_whitespace()
                .map(|x| x.parse())
                .collect::<Result<Vec<_>, _>>()
                .map_err(|e| Error::ParseFloat(line, e))?;

            vertex.push(Vert {
                pos: [parts[0], parts[1], parts[2]],
                norm: [parts[3], parts[4], parts[5]],
            });
        }

        let idx_line = 2 + vert_count;
        buf.clear();
        input.read_line(&mut buf).map_err(
            |e| Error::Io(idx_line, e),
        )?;
        let index_count: usize = {
            let parts: Vec<_> = buf.trim().split_whitespace().collect();
            if parts.len() != 2 || parts[0] != "index" {
                return Err(Error::MissingIndexCount(idx_line));
            }
            parts[1].parse().map_err(|e| Error::ParseInt(idx_line, e))?
        };

        let mut index = Vec::with_capacity(index_count);
        for line in (0..index_count).map(|x| x + idx_line) {
            buf.clear();
            input.read_line(&mut buf).map_err(|e| Error::Io(line, e))?;
            for item in buf.trim().split_whitespace().map(|x| x.parse()) {
                index.push(item.map_err(|e| Error::ParseInt(line, e))?)
            }
        }

        Ok(ModelData { vertex, index })
    }
}

pub struct Model {
    pub vertex: glium::VertexBuffer<Vert>,
    pub index: glium::IndexBuffer<u16>,
}

#[derive(Debug)]
pub enum Error {
    Io(usize, std::io::Error),
    ParseInt(usize, std::num::ParseIntError),
    ParseFloat(usize, std::num::ParseFloatError),
    BadCount(usize),
    MissingVertCount(usize),
    MissingIndexCount(usize),
}
