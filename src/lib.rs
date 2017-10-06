#[macro_use]
extern crate live_reload;

mod host;
use host::{Axis, Host};

struct State {}

live_reload! {
    host: Host;
    state: State;
    init: init;
    reload: reload;
    update: update;
    unload: unload;
    deinit: deinit;
}

fn init(_host: &mut Host, _state: &mut State) {}
fn reload(_host: &mut Host, _state: &mut State) {}
fn update(host: &mut Host, _state: &mut State) -> live_reload::ShouldQuit {
    host.clear_color[0] = 0.8;
    host.clear_color[1] = 0.8;
    host.clear_color[2] = 0.8;
    host.clear_color[3] = 1.0;
    if host.get_axis(Axis::MoveUD) > 0.0 {
        host.clear_color[0] = 1.0;
        host.clear_color[2] = 0.6;
    } else if host.get_axis(Axis::MoveUD) < 0.0 {
        host.clear_color[0] = 0.6;
        host.clear_color[2] = 1.0;
    }
    host.pos[2] -= host.get_axis(Axis::MoveUD) * 0.05;
    live_reload::ShouldQuit::No
}
fn unload(_host: &mut Host, _state: &mut State) {}
fn deinit(_host: &mut Host, _state: &mut State) {}
