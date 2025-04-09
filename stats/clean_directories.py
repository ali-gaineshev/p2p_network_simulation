import os


def delete_all_files_in_subdirectories():
    root_dir = os.getcwd()
    if ("stats" not in root_dir):
        print("Please run this script from the 'stats' directory.")
        return
    deleted_count = 0
    error_count = 0

    # walk through all directories
    for root, _, files in os.walk(root_dir):

        if root == root_dir:
            continue

        for file in files:
            file_path = os.path.join(root, file)
            try:
                os.remove(file_path)
                print(f"Deleted: {file_path}")
                deleted_count += 1
            except Exception as e:
                print(f"Error deleting {file_path}: {e}")
                error_count += 1

    print(
        f"\nOperation complete. Deleted {deleted_count} files, encountered {error_count} errors.")


if __name__ == "__main__":
    delete_all_files_in_subdirectories()
