use regex::Regex;
use std::fs::DirEntry;
use chrono::{Date, Local};
use chrono::prelude::*;

const SS_FORMAT: &str = r#"[0-9]+_([0-1][0-9])([0-3][0-9])([0-9]{4})_.+(\.png|jpg)"#;
const OUTPUT_FOLDER: &str = "_screenshots";

#[derive(Debug)]
struct ScreenshotData {
    file: DirEntry,
    timestamp: Date<Local>,
}

impl ScreenshotData {
    pub fn from_direntry(file: DirEntry) -> Self {
        let timestamp = date_from_direntry(&file)
            .expect("Malformed timestamp");
        Self {
            file,
            timestamp,
        }
    }
}

fn date_from_direntry(file: &DirEntry) -> Option<Date<Local>> {
    let name = format!("{:#?}", file.file_name());
    let rgx = Regex::new(SS_FORMAT)
        .expect("Error constructing regex");
    
    let parts = rgx.captures(name.as_str());

    match parts {
        Some(c) => {
            let day: u32 = c.get(1)
                .expect("Couldn't collect day from filename")
                .as_str()
                .parse()
                .expect("Couldn't parse day to u32");
            let month: u32 = c.get(2)
                .expect("Couldn't collect month from filename")
                .as_str()
                .parse()
                .expect("Couldn't parse month to u32");
            let year: i32 = c.get(3)
                .expect("Couldn't collect year from filename")
                .as_str()
                .parse()
                .expect("Couldn't parse year to i32");
            //println!("{:?}", time);
            let date = Local.ymd(year, month, day);
            Some(date)
        },
        None => None,
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error + Send + Sync>> {
    let rgx = Regex::new(SS_FORMAT)
        .expect("Error constructing regex");

    let files = std::fs::read_dir("screenshots")
        .expect("Error opening dir")
        .filter(|f| {
            match f {
                Ok(file) => {
                    let name = format!("{:?}", file.file_name());
                    rgx.is_match(name.as_str())
                },
                Err(e) => {
                    println!("{:?}", e);
                    false
                },
            }
        })
        .map(|f| ScreenshotData::from_direntry(f.expect("Uknown error during unwrapping direntries")));

    
    if let Err(e) = std::fs::create_dir(OUTPUT_FOLDER) {
        return Err(e.into_inner().expect("Error getting IO Error"));
    }

    for f in files {
        let new_name = format!("ffxiv_{}_1.png", 
            f.timestamp.format("%Y-%m-%d"),
        );
        match std::fs::copy(f.file.path(), format!("{}/{}", OUTPUT_FOLDER, new_name)) {
            Ok(ok) => println!("Copied {:?}", ok),
            Err(e) => println!("Failed to copy {:?}", e),
        }
    }

    Ok(())
}
