use std::fs;
use std::fs::File;
use std::io::Write;
use std::path::Path;
use ascii::AsciiStr;
use ascii_literal::ascii_literal;

const TEST_DATA: &AsciiStr = ascii_literal!("This is some test data. ");
const REPEATS: usize = 1000;

fn main() {
    let path = Path::new("2.DAT");
    if path.is_file() { fs::remove_file(path).unwrap(); }
    let mut file = File::create(path).unwrap();
    for _ in 0..REPEATS {
        file.write_all(TEST_DATA.as_bytes()).unwrap();
    }
}
