#[macro_use]
extern crate glium;
extern crate cgmath;
extern crate live_reload;
extern crate notify;

use live_reload::ShouldQuit;
use glium::Surface;
use cgmath::{conv, Matrix4};
use cgmath::prelude::*;

mod host;
mod input_handler;
mod asset;

use host::{Host, RenderCommand};

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

    let mut manager = asset::AssetManager::new(&display);
    let model = manager.load_model("assets/models/cubes").expect(
        "Load cubes model",
    );
    let program = manager.load_shader("assets/shaders/basic").expect(
        "Load basic shader",
    );

    let mut input = input_handler::Input::new();
    let mut projection = conv::array4x4(input.projection());
    let mut view_transform = conv::array4x4(Matrix4::from_scale(1.0f32));

    let mut running = true;
    while running {
        app.reload().expect("Should safely reload!");
        manager.check_changes();

        running = input.handle_input(&mut events_loop, &mut app);
        projection = conv::array4x4(input.projection());

        if app.update() == ShouldQuit::Yes {
            running = false;
        }

        let mut frame = display.draw();
        for cmd in &app.host().render_queue {
            match *cmd {
                RenderCommand::ClearColor(col) => frame.clear_color(col[0], col[1], col[2], col[3]),
                RenderCommand::ClearDepth(d) => frame.clear_depth(d),
                RenderCommand::Model(id, transform) => {
                    let _ = id;
                    let uniforms =
                        uniform!{
                            proj_from_view: projection,
                            world_from_object: conv::array4x4(transform),
                            view_from_world: view_transform,
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
                        .draw(
                            manager.vertex_buf(id.into()),
                            manager.index_buf(id.into()),
                            manager.shader(program),
                            &uniforms,
                            &params,
                        )
                        .unwrap();
                }
            }
        }
        app.host_mut().render_queue.clear();
        frame.finish().expect("Should render?");
    }
}
