#[macro_use]
extern crate live_reload;
extern crate cgmath;

mod host;
use host::{Axis, Host};

struct State {
    pos: cgmath::Vector3<f32>,
}

live_reload! {
    host: Host;
    state: State;
    init: init;
    reload: reload;
    update: update;
    unload: unload;
    deinit: deinit;
}

fn init(_host: &mut Host, state: &mut State) {
    state.pos = cgmath::vec3(0.0, 0.0, 0.0);
}

fn reload(_host: &mut Host, _state: &mut State) {}

fn update(host: &mut Host, state: &mut State) -> live_reload::ShouldQuit {
    let (mut r, g, mut b) = (0.8, 0.8, 0.8);
    if host.get_axis(Axis::MoveUD) > 0.0 {
        r = 1.0;
        b = 0.6;
        state.pos.z += 0.05;
    } else if host.get_axis(Axis::MoveUD) < 0.0 {
        r = 0.6;
        b = 1.0;
        state.pos.z -= 0.05;
    }

    let mat = cgmath::Matrix4::from_translation(state.pos);

    host.send_render_command(host::RenderCommand::ClearColor([r, g, b, 1.0]));
    host.send_render_command(host::RenderCommand::ClearDepth(1.0));
    host.send_render_command(host::RenderCommand::Model(host::ModelId(0), mat));
    live_reload::ShouldQuit::No
}

fn unload(_host: &mut Host, _state: &mut State) {}
fn deinit(_host: &mut Host, _state: &mut State) {}
