use regex::Regex;
use std::fs::DirEntry;
use std::time::Instant;

const SS_FORMAT: &str = r#"[0-9]+_([0-1][0-9])([0-3][0-9])([0-9]{4})_.+(\.png|jpg)"#;

#[derive(Debug)]
struct ScreenshotData {
    file: DirEntry,
    timestamp: Timestamp,
}

impl ScreenshotData {
    pub fn from_direntry(file: DirEntry) -> Self {
        let timestamp = Timestamp::from_direntry(&file)
            .expect("Malformed timestamp");
        Self {
            file,
            timestamp,
        }
    }
}

#[derive(Debug)]
struct Timestamp(String);

impl Timestamp {
    pub fn from_direntry(file: &DirEntry) -> Option<Self> {
        let name = format!("{:?}", file.file_name());
        let rgx = Regex::new(SS_FORMAT)
            .expect("Error constructing regex");
        
        let parts = rgx.captures(name.as_str());

        match parts {
            Some(c) => {
                let day = c.get(1)
                    .expect("Couldn't collect day from filename")
                    .as_str();
                let month = c.get(2)
                    .expect("Couldn't collect month from filename")
                    .as_str();
                let year = c.get(3)
                    .expect("Couldn't collect year from filename")
                    .as_str();
                //println!("{:?}", time);
                Some(Self(format!("{}-{}-{}", year, month, day)))
            },
            None => None,
        }
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
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

    
    for f in files {
        println!("{:?}", f);
    }

    Ok(())
}
