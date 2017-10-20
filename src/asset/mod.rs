use std::io;
use std::collections::HashMap;
use std::path::{Path, PathBuf};
use std::sync::mpsc::{channel, Receiver};
use std::time::Duration;

use notify::{RecommendedWatcher, Watcher, RecursiveMode, DebouncedEvent, watcher};
use glium;

mod shader;
mod model;

pub use self::shader::ShaderHandle;
pub use self::model::{ModelError, ModelHandle};

#[derive(Debug)]
pub enum AssetError {
    File(PathBuf, io::Error),
    Shader(PathBuf, glium::ProgramCreationError),
    Model(PathBuf, ModelError),
}

pub struct AssetManager {
    watcher: RecommendedWatcher,
    events: Receiver<DebouncedEvent>,
    model_names: HashMap<PathBuf, ModelHandle>,
    models: Vec<model::ModelResource>,
    shader_names: HashMap<PathBuf, ShaderHandle>,
    shaders: Vec<shader::ShaderResource>,
}

impl AssetManager {
    pub fn new() -> AssetManager {
        let (tx, rx) = channel();
        let mut watcher = watcher(tx, Duration::from_secs(1)).unwrap();
        watcher.watch("assets", RecursiveMode::Recursive).unwrap();
        AssetManager {
            watcher: watcher,
            events: rx,
            model_names: HashMap::new(),
            models: Vec::new(),
            shader_names: HashMap::new(),
            shaders: Vec::new(),
        }
    }

    pub fn check_changes(&self) {
        for change in self.events.try_iter() {
            match change {
                e => println!("{:?}", e),
            }
        }
    }

    pub fn load_shader<P: AsRef<Path>>(
        &mut self,
        display: &glium::Display,
        name: P,
    ) -> Result<ShaderHandle, AssetError> {
        let shader = shader::load_shader(display, &name)?;

        let len = self.shaders.len() as u32;
        let id = self.shader_names
            .entry(name.as_ref().into())
            .or_insert_with(|| unsafe { ShaderHandle::from_raw_parts(len) });

        if id.raw_id() as usize >= self.shaders.len() {
            self.shaders.push(shader);
        } else {
            self.shaders[id.raw_id() as usize] = shader;
        }

        Ok(*id)
    }

    pub fn load_model<P: AsRef<Path>>(
        &mut self,
        display: &glium::Display,
        name: P,
    ) -> Result<ModelHandle, AssetError> {
        let model = model::load_model(display, &name)?;

        let len = self.models.len() as u32;
        let id = self.model_names
            .entry(name.as_ref().into())
            .or_insert_with(|| unsafe { ModelHandle::from_raw_parts(len) });

        if id.raw_id() as usize >= self.models.len() {
            self.models.push(model);
        } else {
            self.models[id.raw_id() as usize] = model;
        }

        Ok(*id)
    }

    pub fn vertex_buf(&self, id: ModelHandle) -> &glium::VertexBuffer<model::Vert> {
        &self.models[id.raw_id() as usize].vertex
    }

    pub fn index_buf(&self, id: ModelHandle) -> &glium::IndexBuffer<u32> {
        &self.models[id.raw_id() as usize].index
    }

    pub fn shader(&self, id: ShaderHandle) -> &glium::Program {
        &self.shaders[id.raw_id() as usize].program
    }
}
