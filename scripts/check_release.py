#! /bin/python


import argparse
import json
import urllib.request
import re
from pathlib import Path

def main() -> None:
    parser = argparse.ArgumentParser(
        description="List undocumented PRs"
    )
    parser.add_argument("-v", "--version", type=str, required=True)
    args = parser.parse_args()
    
    match = re.search("([0-9]+\.[0-9]+\.[0-9]+)", args.version)
    
    if not match:
        print("invalid version")
        exit(1)
    
    version = "v" + match.group(1)
    
    print("looking for milestone for " + version + " ...")
    
    
    with urllib.request.urlopen("https://egit.irs.uni-stuttgart.de/api/v1/repos/fsfw/fsfw/milestones?name=" + version) as milestone_json:
        milestones = json.load(milestone_json)
        if (len(milestones) == 0):
            print("did not find any milestone")
            exit(1)
        if (len(milestones) > 1):
            print("found multiple milestons")
        milestone_title = milestones[0]['title']
        milestone = str(milestones[0]['id'])
        print("Using Milestone \""+ milestone_title + "\" with id " + milestone)
    
    milestone_prs = []
    
    page = 1
    last_count = 1;
    while last_count != 0:
        with urllib.request.urlopen("https://egit.irs.uni-stuttgart.de/api/v1/repos/fsfw/fsfw/pulls?state=closed&milestone=" + str(milestone) + "&limit=100&page=" + str(page)) as pull_requests_json:
            pull_requests = json.load(pull_requests_json)
            for pr in pull_requests:
                milestone_prs.append({'number': str(pr['number']), 'title' : pr['title']})
            page += 1
            last_count = len(pull_requests)
    
    print("Found " + str(len(milestone_prs)) + " closed PRs in Milestone")
    
    print("looking for CHANGELOG.md ...")
    
    path = Path(".")
    
    files = list(path.glob("CHANGELOG.md"))
    
    if (len(files) != 1):
        files = list(path.glob("../CHANGELOG.md"))
        
    if (len(files) != 1):
        print("did not find CHANGELOG.md. Run script in either root directory or scripts subfolder.")
        exit(1)
        
    print("Scanning CHANGELOG.md ...")
    
    changelog_prs = []
    
    with open(files[0]) as changelog:
        line = changelog.readline()
        while (line):
            #print("line: " + line)
            match = re.search("\#.+(v[0-9]+\.[0-9]+\.[0-9]+)", line)
            if (match):
                if match.group(1) == version:
                    #print("found version")
                    line = changelog.readline()
                    continue
                else:
                    #print("done with " + match.group(1))
                    break
            
            match = re.search("PR: https://egit\.irs\.uni-stuttgart\.de/fsfw/fsfw/pulls/([0-9]+)", line)
            if match:
                changelog_prs.append(match.group(1))
                
            line = changelog.readline()
    
    print("Found " + str(len(changelog_prs)) + " PRs in CHANGELOG.md")
    
    print("")
    
    copy_array = changelog_prs.copy()
    print("PRs in CHANGELOG.md that are not in Milestone:")
    for pr in milestone_prs:
        if pr['number'] in copy_array:
            copy_array.remove(pr['number'])
    for pr in copy_array:
        print("https://egit.irs.uni-stuttgart.de/fsfw/fsfw/pulls/" + pr)

    print("")

    print("PRs in milestone that are not in CHANGELOG.md:")
    
    for pr in milestone_prs:
        if pr['number'] not in changelog_prs:
            print("- " + pr['title'] + "\n  PR: https://egit.irs.uni-stuttgart.de/fsfw/fsfw/pulls/" + pr['number'])

main()
