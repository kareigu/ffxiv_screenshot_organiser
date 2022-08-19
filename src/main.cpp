#include <algorithm>
#include <fmt/core.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <regex>

struct Screenshot {
  std::string formatted_name;
  std::filesystem::directory_entry file;
};

constexpr auto SCREENSHOT_FOLDER = "./screenshots";
constexpr auto OUTPUT_FOLDER = "./_screenshots";
constexpr auto REGEX_STRING = "^ffxiv_([0-3][0-9])([0-1][0-9])([0-9]{4})_([0-2][0-9])([0-6][0-9])([0-6][0-9])_([0-9]{3}).png$";


int main(int argc, char** argv) {
  if (!std::filesystem::exists(SCREENSHOT_FOLDER)) {
    fmt::print("Place the executable next to your screenshots folder\n");
    return EXIT_FAILURE;
  }


  std::vector<Screenshot> screenshots{};

  std::regex screenshot_regex(REGEX_STRING);

  for (const auto& file : std::filesystem::directory_iterator(SCREENSHOT_FOLDER)) {
    if (!file.is_regular_file())
      continue;

    std::smatch matches;
    const auto filename = file.path().filename().string();
    if (std::regex_match(filename, matches, screenshot_regex)) {
      const auto& day = matches[1].str();
      const auto& month = matches[2].str();
      const auto& year = matches[3].str();
      const auto& hour = matches[4].str();
      const auto& minute = matches[5].str();
      const auto& second = matches[6].str();
      const auto& id = matches[7].str();

      std::string new_name = fmt::format("ffxiv_{}-{}-{}--{}-{}-{}_{}.png", year, month, day, hour, minute, second, id);

      screenshots.push_back({new_name, file});
    } 
  }

  fmt::print("Found {} screenshots to organise\n", screenshots.size());

  if (!std::filesystem::exists(OUTPUT_FOLDER))
    if (!std::filesystem::create_directory(OUTPUT_FOLDER)) {
      fmt::print("Couldn't create output folder\n");
      return EXIT_FAILURE;
    }

  for (const auto& screenshot : screenshots) {
    fmt::print("{} -> {}\n", screenshot.file.path().filename().string(), screenshot.formatted_name);

    std::string output_path_string = fmt::format("{}/{}", OUTPUT_FOLDER, screenshot.formatted_name);
    std::filesystem::path output_path(output_path_string);

    std::error_code error{};
    std::filesystem::copy_file(screenshot.file.path(), output_path, error);

    if (error) {
      fmt::print("Copy unsuccesfull\n");
      continue;
    }


    const auto original_write_time = std::filesystem::last_write_time(screenshot.file.path());
    std::filesystem::last_write_time(output_path, original_write_time);
    fmt::print("Copied successfully\n");
  }

  return EXIT_SUCCESS;
}