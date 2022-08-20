#include <algorithm>
#include <fmt/core.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <regex>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/screen.hpp>

struct Screenshot {
  std::string formatted_name;
  std::filesystem::directory_entry file;
};

constexpr auto SCREENSHOT_FOLDER = "./screenshots";
constexpr auto OUTPUT_FOLDER = "./_screenshots";
constexpr auto REGEX_STRING = "^ffxiv_([0-3][0-9])([0-1][0-9])([0-9]{4})_([0-2][0-9])([0-6][0-9])([0-6][0-9])_([0-9]{3}).png$";

inline bool check_answer(char input) {
  return input == 'y' || input == 'Y';
}

bool wait_for_confirm(char default_value) {
  while (true) {
    char input;

    std::cin >> input;

    //std::cin.ignore();

    if (input == '\n') input = default_value;
    if (input == 'y' || input == 'Y') return true;
    if (input == 'n' || input == 'N') return false;
  }
}


int main(int argc, char** argv) {
  if (!std::filesystem::exists(SCREENSHOT_FOLDER)) {
    fmt::print("Place the executable next to your screenshots folder\n");
    return EXIT_FAILURE;
  }


  std::vector<Screenshot> screenshots{};
  std::vector<std::string> invalid_files{};

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
    } else {
      invalid_files.push_back(file.path().filename().string());
    }
  }

  if (invalid_files.size() > 0) {
    fmt::print("Found {} invalid filename(s), would you like to list them? (y, N) ", invalid_files.size());

    if (wait_for_confirm('N')) {
      for (const auto& invalid : invalid_files) {
        fmt::print("{}\n", invalid);
      }

      fmt::print("Continue processing files? (Y, n) ");

      if (!wait_for_confirm('Y'))
        return EXIT_SUCCESS;
    }
  }


  fmt::print("Found {} screenshots to organise\n", screenshots.size());

  if (screenshots.size() == 0)
    return EXIT_FAILURE;

  if (!std::filesystem::exists(OUTPUT_FOLDER))
    if (!std::filesystem::create_directory(OUTPUT_FOLDER)) {
      fmt::print("Couldn't create output folder\n");
      return EXIT_FAILURE;
    }

  std::string reset_position;
  size_t current_file = 0;
  size_t failed_copies = 0;
  const size_t total_files = screenshots.size();

  std::vector<std::string> recent_filenames(5);

  for (const auto& screenshot : screenshots) {
    current_file++;
    std::string filenames = fmt::format("{} -> {}\n", screenshot.file.path().filename().string(), screenshot.formatted_name);
    std::string progress_meter = fmt::format(" {}/{}", current_file, total_files);

    recent_filenames[0] = recent_filenames[1];
    recent_filenames[1] = recent_filenames[2];
    recent_filenames[2] = recent_filenames[3];
    recent_filenames[3] = recent_filenames[4];
    recent_filenames[4] = filenames;


    std::string output_path_string = fmt::format("{}/{}", OUTPUT_FOLDER, screenshot.formatted_name);
    std::filesystem::path output_path(output_path_string);

    std::error_code error{};
    std::filesystem::copy_file(screenshot.file.path(), output_path, error);

    if (error) failed_copies++;

    auto progress_gauge = ftxui::hbox({
      ftxui::text("Processing files: "),
      ftxui::gauge(current_file / total_files) | ftxui::flex,
      ftxui::text(progress_meter),
      ftxui::text(" | "),
      ftxui::text(fmt::format("({})", failed_copies)) | ftxui::color(ftxui::Color::Red1),
    });

    ftxui::Elements vstack{};
    for (const auto& filenames : recent_filenames) {
      auto color = error ? ftxui::color(ftxui::Color::Red1) : ftxui::color(ftxui::Color::Green1);
      vstack.push_back(ftxui::text(filenames) | color);
    }

    vstack.push_back(progress_gauge);

    auto copy_progress = ftxui::vbox(vstack);

    auto screen = ftxui::Screen(100, 6);
    ftxui::Render(screen, copy_progress);

    std::cout << reset_position;
    screen.Print();
    reset_position = screen.ResetPosition();

    if (error) {
      continue;
    }


    const auto original_write_time = std::filesystem::last_write_time(screenshot.file.path());
    std::filesystem::last_write_time(output_path, original_write_time);


    //fmt::print("Copied successfully\n");
    
  }
  fmt::print("\n");

  return EXIT_SUCCESS;
}