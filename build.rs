extern crate cmake;

fn main() {
    let dst = cmake::build("");

    println!("cargo:rustc-link-search=native={}", dst.display());
    println!("cargo:rustc-link-lib=static=axal");

    // TODO: Generate this from the cmake file
    println!("cargo:rustc-link-lib=dylib=GL");
    println!("cargo:rustc-link-lib=dylib=Qt5Core");
    println!("cargo:rustc-link-lib=dylib=Qt5Widgets");
    println!("cargo:rustc-link-lib=dylib=Qt5Gui");
    println!("cargo:rustc-link-lib=dylib=stdc++");
}
