use regex::Regex;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let rgx = Regex::new(r#"(_[0-1][0-9][0-3][0-9][0-9]{4}_).+(\.png|jpg)"#)
        .expect("Error constructing regex");

    let files = std::fs::read_dir("screenshots")
        .expect("Error opening dir")
        .filter(|f| {
            match f {
                Ok(file) => rgx.is_match(file.file_name())
            }
        });
    
    for f in files {
        println!("{:?}", f);
    }

    Ok(())
}
