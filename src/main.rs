use regex::Regex;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let rgx = Regex::new(r#"[0-9]+_([0-1][0-9][0-3][0-9][0-9]{4})_.+(\.png|jpg)"#)
        .expect("Error constructing regex");

    let files = std::fs::read_dir("screenshots")
        .expect("Error opening dir")
        .filter(|f| {
            match f {
                Ok(file) => {
                    let name = file.file_name();
                    rgx.is_match(format!("{:?}", name).as_str())
                },
                Err(e) => {
                    println!("{:?}", e);
                    false
                },
            }
        });
    
    for f in files {
        println!("{:?}", f);
    }

    Ok(())
}
