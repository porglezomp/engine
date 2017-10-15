#[macro_use]
extern crate glium;
extern crate cgmath;
extern crate live_reload;

use live_reload::ShouldQuit;
use glium::Surface;
use std::io::Read;
use cgmath::conv;
use std::path::{Path, PathBuf};
// use cgmath::prelude::*;

mod host;
mod input_handler;
mod model;

use host::Host;

pub fn main() {
    let mut events_loop = glium::glutin::EventsLoop::new();
    let window = glium::glutin::WindowBuilder::new()
        .with_dimensions(1280, 720)
        .with_title("Garden");
    let context = glium::glutin::ContextBuilder::new()
        .with_vsync(true)
        .with_depth_buffer(32);
    let display =
        glium::Display::new(window, context, &events_loop).expect("Should create display!");

    let host = Host::new();
    let mut app = live_reload::Reloadable::new("target/debug/libgame.dylib", host)
        .expect("Should load!");

    let file = std::fs::File::open("assets/models/cubes.model").unwrap();
    let mut buf_read = std::io::BufReader::new(file);
    let model_data = model::ModelData::load(&mut buf_read).expect("Should load");

    let model = model::Model {
        vertex: glium::vertex::VertexBuffer::new(&display, &model_data.vertex).unwrap(),
        index: glium::index::IndexBuffer::new(
            &display,
            glium::index::PrimitiveType::TrianglesList,
            &model_data.index,
        ).unwrap(),
    };

    let program = load_shader(&display, "assets/shaders/basic").unwrap();

    let proj = conv::array4x4(cgmath::perspective(
        cgmath::Deg(80.0f32),
        1280.0 / 720.0,
        0.1,
        500.0,
    ));

    let mut input = input_handler::Input::new();

    let mut running = true;
    while running {
        app.reload().expect("Should safely reload!");

        running = input.handle_input(&mut events_loop, &mut app);

        if app.update() == ShouldQuit::Yes {
            running = false;
        }

        let mut frame = display.draw();
        let view_transform = <cgmath::Matrix4<f32> as cgmath::One>::one();
        for cmd in &app.host().render_queue {
            match *cmd {
                host::RenderCommand::ClearColor(col) => {
                    frame.clear_color(col[0], col[1], col[2], col[3])
                }
                host::RenderCommand::ClearDepth(d) => frame.clear_depth(d),
                host::RenderCommand::Model(id, transform) => {
                    let _ = id;
                    let uniforms =
                        uniform!{
                            proj_from_view: proj,
                            world_from_object: conv::array4x4(transform),
                            view_from_world: conv::array4x4(view_transform),
                        };

                    let params = glium::DrawParameters {
                        depth: glium::Depth {
                            test: glium::DepthTest::IfLess,
                            write: true,
                            ..Default::default()
                        },
                        ..Default::default()
                    };
                    frame
                        .draw(&model.vertex, &model.index, &program, &uniforms, &params)
                        .unwrap();
                }
            }
        }
        app.host_mut().render_queue.clear();
        frame.finish().expect("Should render?");
    }
}

#[derive(Debug)]
enum AssetError {
    FileError(PathBuf, std::io::Error),
    ShaderError(PathBuf, glium::ProgramCreationError),
}

fn load_shader<P: AsRef<Path>>(
    display: &glium::Display,
    name: P,
) -> Result<glium::Program, AssetError> {
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
    Ok(glium::program::Program::from_source(
        display,
        &vert_src,
        &frag_src,
        None,
    ).map_err(|e| {
        AssetError::ShaderError(PathBuf::from(name.as_ref()), e)
    })?)
}
