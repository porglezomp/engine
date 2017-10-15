#[macro_use]
extern crate live_reload;
extern crate cgmath;

use cgmath::prelude::*;
use cgmath::{Vector3, Matrix4, vec3, Deg};

mod host;
use host::{Axis, Host, RenderCommand, ModelId};

#[repr(C)]
struct State {
    pos: Vector3<f32>,
    horiz: f32,
    vert: f32,
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
    state.horiz = 0.0;
    state.vert = 0.0;
}

fn reload(_host: &mut Host, state: &mut State) {
    state.horiz = 0.0;
    state.vert = 0.0;
}

fn update(host: &mut Host, state: &mut State) -> live_reload::ShouldQuit {
    const SPEED: f32 = 0.1;
    let forwards = host.get_axis(Axis::MoveUD);
    let sideways = host.get_axis(Axis::MoveLR);
    let movement = vec3(sideways, 0.0, -forwards) * SPEED;
    state.horiz -= host.get_axis(Axis::LookLR);
    state.vert -= host.get_axis(Axis::LookUD);

    let rot = Matrix4::from_axis_angle(vec3(0.0, 0.0, 1.0), Deg(state.horiz)) *
        Matrix4::from_axis_angle(vec3(1.0, 0.0, 0.0), Deg(state.vert));
    state.pos += rot.transform_vector(movement);

    let mat = rot.transpose() * Matrix4::from_translation(-state.pos);

    host.send_render_command(RenderCommand::ClearColor([1.0, 0.1, 0.0, 1.0]));
    host.send_render_command(RenderCommand::ClearDepth(1.0));
    host.send_render_command(RenderCommand::Model(ModelId(0), mat));
    live_reload::ShouldQuit::No
}

fn unload(_host: &mut Host, _state: &mut State) {}
fn deinit(_host: &mut Host, _state: &mut State) {}
