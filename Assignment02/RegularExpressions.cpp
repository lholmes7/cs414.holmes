#include <iostream>
#include <regex>
#include <string>
#include <vector>

int main() {
    // Patterns
    std::regex cpp_id("^[A-Za-z_][A-Za-z0-9_]*$");
    std::regex phone("^(\\(\\d{3}\\)\\s|\\d{3}-)\\d{3}-\\d{4}$");
    std::regex floating("^[+-]?\\d+(\\.\\d+)?$");
    std::regex bin_pal("^(000|111|010|101|0110|1001|0000|1111)$");

    // Test sets
    std::vector<std::string> cpp_ids = { "e1", "_id", "4go" };
    std::vector<std::string> phones = { "245) 428-1725", "757-254-2348", "10-7281-9432" };
    std::vector<std::string> floats = { "3.14", "-", "+.1" };
    std::vector<std::string> bin_pals = { "101", "0110", "1100" };

    auto test_regex = [](const std::regex& pattern,
        const std::vector<std::string>& tests,
        const std::string& name) {
            std::cout << "\nTesting " << name << ":\n";
            for (const auto& s : tests) {
                bool match = std::regex_match(s, pattern);
                std::cout << s << " -> " << (match ? "true" : "false") << "\n";
            }
        };

    // Run tests
    test_regex(cpp_id, cpp_ids, "C++ identifiers");
    test_regex(phone, phones, "Phone numbers");
    test_regex(floating, floats, "Floating point numbers");
    test_regex(bin_pal, bin_pals, "Binary palindromes");

    return 0;
}