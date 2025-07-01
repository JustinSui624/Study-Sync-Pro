#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <algorithm>
namespace std;
struct Profile {
    int ID;
    string Name;
    int GradYear;
    string CurrentClasses;
};
int countSharedClasses(const std::string& a, const std::string& b) {
    unordered_set<std::string> setA, setB;
    stringstream ssA(a), ssB(b);
    string item;

    while (getline(ssA, item, ',')) {
        setA.insert(item);
    }
    while (getline(ssB, item, ',')) {
        if (setA.count(item)) setB.insert(item);
    }
    return setB.size();
}
void matchProfiles(const vector<Profile>& profiles, int threshold = 2) {
    for (size_t i = 0; i < profiles.size(); ++i) {
        for (size_t j = i + 1; j < profiles.size(); ++j) {
            int score = 0;
            if (profiles[i].GradYear == profiles[j].GradYear) {
                score += 1;
            }
            int shared = countSharedClasses(profiles[i].CurrentClasses, profiles[j].CurrentClasses);
            score += shared;
            if (score >= threshold) {
                std::cout << "Match: " << profiles[i].Name << " = " << profiles[j].Name
                          << " (Score: " << score << ")\n";
            }
        }
    }
}
