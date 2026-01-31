#!/usr/bin/env bash
# Version bump script for task-tracker
# Usage: ./scripts/bump-version.sh [major|minor|patch|<version>]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
VERSION_FILE="$ROOT_DIR/VERSION"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to display usage
usage() {
    echo "Usage: $0 [major|minor|patch|<version>]"
    echo ""
    echo "Examples:"
    echo "  $0 major      # Bump major version (1.0.0 -> 2.0.0)"
    echo "  $0 minor      # Bump minor version (1.0.0 -> 1.1.0)"
    echo "  $0 patch      # Bump patch version (1.0.0 -> 1.0.1)"
    echo "  $0 1.2.3      # Set specific version"
    exit 1
}

# Function to parse version
parse_version() {
    local version=$1
    echo "$version" | grep -qE '^[0-9]+\.[0-9]+\.[0-9]+$' || {
        echo -e "${RED}Error: Invalid version format. Expected: X.Y.Z${NC}" >&2
        exit 1
    }
    
    IFS='.' read -r major minor patch <<< "$version"
    echo "$major $minor $patch"
}

# Function to increment version
increment_version() {
    local current=$1
    local bump_type=$2
    
    read -r major minor patch <<< "$(parse_version "$current")"
    
    case $bump_type in
        major)
            major=$((major + 1))
            minor=0
            patch=0
            ;;
        minor)
            minor=$((minor + 1))
            patch=0
            ;;
        patch)
            patch=$((patch + 1))
            ;;
        *)
            echo -e "${RED}Error: Invalid bump type: $bump_type${NC}" >&2
            exit 1
            ;;
    esac
    
    echo "$major.$minor.$patch"
}

# Function to update version in files
update_version_in_files() {
    local new_version=$1
    
    echo -e "${YELLOW}Updating version to $new_version...${NC}"
    
    # Update VERSION file
    echo "$new_version" > "$VERSION_FILE"
    echo -e "${GREEN}✓ Updated VERSION file${NC}"
    
    # Update CMakeLists.txt
    read -r major minor patch <<< "$(parse_version "$new_version")"
    # Use temporary file for cross-platform sed compatibility
    sed "s/^set(TaskTracker_Version_Major .*)$/set(TaskTracker_Version_Major $major)/" "$ROOT_DIR/CMakeLists.txt" > "$ROOT_DIR/CMakeLists.txt.tmp" && mv "$ROOT_DIR/CMakeLists.txt.tmp" "$ROOT_DIR/CMakeLists.txt"
    sed "s/^set(TaskTracker_Version_Minor .*)$/set(TaskTracker_Version_Minor $minor)/" "$ROOT_DIR/CMakeLists.txt" > "$ROOT_DIR/CMakeLists.txt.tmp" && mv "$ROOT_DIR/CMakeLists.txt.tmp" "$ROOT_DIR/CMakeLists.txt"
    sed "s/^set(TaskTracker_Version_Patch .*)$/set(TaskTracker_Version_Patch $patch)/" "$ROOT_DIR/CMakeLists.txt" > "$ROOT_DIR/CMakeLists.txt.tmp" && mv "$ROOT_DIR/CMakeLists.txt.tmp" "$ROOT_DIR/CMakeLists.txt"
    echo -e "${GREEN}✓ Updated CMakeLists.txt${NC}"
    
    # Update flake.nix - more specific pattern targeting pname line context
    sed "s/^\([ ]*pname = \"task-cli\";\)$/\1/" "$ROOT_DIR/flake.nix" > "$ROOT_DIR/flake.nix.tmp" && mv "$ROOT_DIR/flake.nix.tmp" "$ROOT_DIR/flake.nix"
    sed "s/^[ ]*version = \".*\";/          version = \"$new_version\";/" "$ROOT_DIR/flake.nix" > "$ROOT_DIR/flake.nix.tmp" && mv "$ROOT_DIR/flake.nix.tmp" "$ROOT_DIR/flake.nix"
    echo -e "${GREEN}✓ Updated flake.nix${NC}"
    
    # Update .cz.yaml
    sed "s/^  version: .*/  version: $new_version/" "$ROOT_DIR/.cz.yaml" > "$ROOT_DIR/.cz.yaml.tmp" && mv "$ROOT_DIR/.cz.yaml.tmp" "$ROOT_DIR/.cz.yaml"
    echo -e "${GREEN}✓ Updated .cz.yaml${NC}"
}

# Main script
main() {
    if [ $# -ne 1 ]; then
        usage
    fi
    
    # Read current version
    if [ ! -f "$VERSION_FILE" ]; then
        echo -e "${RED}Error: VERSION file not found at $VERSION_FILE${NC}" >&2
        exit 1
    fi
    
    current_version=$(cat "$VERSION_FILE" | tr -d '[:space:]')
    echo -e "Current version: ${GREEN}$current_version${NC}"
    
    # Determine new version
    bump_type=$1
    if [[ "$bump_type" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
        # Specific version provided
        new_version=$bump_type
        echo -e "Setting version to: ${GREEN}$new_version${NC}"
    elif [[ "$bump_type" =~ ^(major|minor|patch)$ ]]; then
        # Bump type provided
        new_version=$(increment_version "$current_version" "$bump_type")
        echo -e "Bumping $bump_type version to: ${GREEN}$new_version${NC}"
    else
        echo -e "${RED}Error: Invalid argument: $bump_type${NC}" >&2
        usage
    fi
    
    # Confirm the change
    read -p "Continue? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Aborted."
        exit 0
    fi
    
    # Update version in all files
    update_version_in_files "$new_version"
    
    echo ""
    echo -e "${GREEN}Version successfully updated to $new_version!${NC}"
    echo ""
    echo "Next steps:"
    echo "  1. Review the changes: git diff"
    echo "  2. Commit the changes: git add -A && git commit -m 'chore: bump version to $new_version'"
    echo "  3. Create a tag: git tag -a v$new_version -m 'Release v$new_version'"
    echo "  4. Push changes: git push && git push --tags"
}

main "$@"
