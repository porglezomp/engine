use std::{io, env};
use std::collections::HashMap;
use std::path::{Path, PathBuf};
use std::sync::mpsc::{channel, Receiver};
use std::time::Duration;
use std::borrow::Cow;

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

pub struct AssetManager<'d> {
    display: &'d glium::Display,
    watcher: RecommendedWatcher,
    events: Receiver<DebouncedEvent>,
    model_names: HashMap<PathBuf, ModelHandle>,
    models: Vec<model::ModelResource>,
    shader_names: HashMap<PathBuf, ShaderHandle>,
    shaders: Vec<shader::ShaderResource>,
}

impl<'d> AssetManager<'d> {
    pub fn new(display: &'d glium::Display) -> AssetManager<'d> {
        let (tx, rx) = channel();
        let mut watcher = watcher(tx, Duration::from_secs(2)).unwrap();
        watcher.watch("assets", RecursiveMode::Recursive).unwrap();
        AssetManager {
            display: display,
            watcher: watcher,
            events: rx,
            model_names: HashMap::new(),
            models: Vec::new(),
            shader_names: HashMap::new(),
            shaders: Vec::new(),
        }
    }

    pub fn check_changes(&mut self) -> Vec<AssetError> {
        let mut errors = Vec::new();
        while let Ok(change) = self.events.try_recv() {
            match change {
                DebouncedEvent::Create(p) |
                DebouncedEvent::Write(p) => {
                    let extension = p.extension();
                    if extension.is_none() {
                        continue;
                    }
                    // @Error
                    let p = p.with_file_name(p.file_stem().unwrap());
                    match extension {
                        Some(ref x) if *x == "vert" || *x == "frag" => {
                            println!("Reloading shader {:?}", p);
                            if let Err(e) = self.load_shader(&p) {
                                println!("Error loading shader {:?}: {:?}", p, e);
                                errors.push(e);
                            }
                        }
                        Some(ref x) if *x == "model" => {
                            println!("Reloading model {:?}", p);
                            if let Err(e) = self.load_model(&p) {
                                println!("Error loading model {:?}: {:?}", p, e);
                                errors.push(e);
                            }
                        }
                        _ => (),
                    }
                }
                _ => (),
            }
        }
        errors
    }

    fn item_path(path: &Path) -> Cow<Path> {
        // @Robustness: Handle asset relative vs fs paths more robustly than this
        if path.is_relative() {
            Cow::Owned(env::current_dir().unwrap().join(path))
        } else {
            Cow::Borrowed(path)
        }
    }

    pub fn load_shader<P: AsRef<Path>>(&mut self, name: P) -> Result<ShaderHandle, AssetError> {
        let name = Self::item_path(name.as_ref());
        let shader = shader::load_shader(self.display, &name)?;

        let len = self.shaders.len() as u32;
        let id = self.shader_names
            // @Error
            .entry(name.as_ref().into())
            .or_insert_with(|| unsafe { ShaderHandle::from_raw_parts(len) });

        if id.raw_id() as usize >= self.shaders.len() {
            self.shaders.push(shader);
        } else {
            self.shaders[id.raw_id() as usize] = shader;
        }

        Ok(*id)
    }

    pub fn load_model<P: AsRef<Path>>(&mut self, name: P) -> Result<ModelHandle, AssetError> {
        let name = Self::item_path(name.as_ref());
        let model = model::load_model(self.display, &name)?;

        let len = self.models.len() as u32;
        let id = self.model_names
            // @Error
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
