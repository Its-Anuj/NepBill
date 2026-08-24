import csv

def process_text_to_csv(input_filename="check.txt", output_filename="output.csv"):
    headers = [
        "id", "rn", "name", "gender", "district", "rank_no",
        "p1", "p2", "p3", "p4", "p5", "p6", "p7", "p8", "p9", "p10", "p11", "p12", "p13", "p14"
    ]
    
    records = []
    matched_rows = []  # List to store matching data for sorting
    
    try:
        with open(input_filename, 'r', encoding='utf-8') as infile:
            for line_num, line in enumerate(infile, 1):
                line = line.strip()
                if not line:
                    continue
                
                tokens = line.split()
                
                # Locate gender position to act as an anchor point
                gender_index = -1
                for idx, token in enumerate(tokens):
                    if token.lower() in ['male', 'female', 'other']:
                        gender_index = idx
                        break
                
                if gender_index == -1:
                    continue
                
                # Extract primary structural columns
                id_val = tokens[0]
                rn_val = f"{tokens[1]} {tokens[2]}"  
                name_val = " ".join(tokens[3:gender_index])
                gender_val = tokens[gender_index]
                district_val = tokens[gender_index + 1]
                rank_no_val = tokens[gender_index + 2]
                
                # Extract p1 through p14 columns
                p_values = tokens[gender_index + 3:]
                while len(p_values) < 14:
                    p_values.append("0")
                p_values = p_values[:14]
                
                # Check ALL 14 fields (p1 through p14) for the value '7'
                if "7" in p_values:
                    # Identify exactly which fields (p1 to p14) matched
                    matched_fields = [f"p{i+1}" for i, val in enumerate(p_values) if val == "7"]
                    fields_str = ", ".join(matched_fields)
                    
                    # Convert rank_no to integer for proper mathematical sorting
                    try:
                        rank_int = int(rank_no_val)
                    except ValueError:
                        rank_int = float('inf')  # Put invalid ranks at the end
                    
                    # Store data as a dictionary to sort later
                    matched_rows.append({
                        "rank_int": rank_int,
                        "rank_no": rank_no_val,
                        "name": name_val,
                        "fields": fields_str
                    })
                
                row = [id_val, rn_val, name_val, gender_val, district_val, rank_no_val] + p_values
                records.append(row)
                
        # Sort matches by rank_int in ascending order
        matched_rows.sort(key=lambda x: x["rank_int"])
        
        # Print the sorted matches to the terminal
        print("--- MATCHED ROWS (p1-p14) IN ASCENDING ORDER OF RANK NO ---")
        idx = 1
        for match in matched_rows:
            print(f"{idx} [Match Found] Name: {match['name']} | Rank No: {match['rank_no']} | Match in: {match['fields']}")
            idx    += 1
        print("------------------------------------------------------------")
                
        # Export processed data cleanly to local CSV disk storage
        with open(output_filename, 'w', newline='', encoding='utf-8') as outfile:
            writer = csv.writer(outfile)
            writer.writerow(headers)
            writer.writerows(records)
            
        print(f"\nProcessing Complete: Logged {len(records)} entries. Found {len(matched_rows)} rows with '7' anywhere in p1-p14.")
        
    except FileNotFoundError:
        print(f"Error: Target text file '{input_filename}' missing from directory.")

if __name__ == '__main__':
    process_text_to_csv()
