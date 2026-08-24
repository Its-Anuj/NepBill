import re
import pandas as pd

def export_all_up_to_rank1010(input_filepath, output_filepath):
    records = []
    
    # Pattern to capture main applicant metadata
    main_pattern = re.compile(
        r'^(?P<sn>\d+)\s+(?P<appid>\d+)\s+(?P<name>.+?)\s+(?P<gender>Male|Female)\s+(?P<rest>.+)$'
    )
    # Pattern to capture district, rank, preferences, and quota
    rest_pattern = re.compile(
        r'^(?P<district>[A-Za-z\s]+?)\s+(?P<rank>\d+)(?:\s+(?P<prefs>(?:\d+\s*)+))?(?P<quota>.*)$'
    )

    with open(input_filepath, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            
            main_match = main_pattern.match(line)
            if main_match:
                row = main_match.groupdict()
                rest_match = rest_pattern.match(row['rest'])
                if rest_match:
                    rest_dict = rest_match.groupdict()
                    prefs_raw = (rest_dict['prefs'] or "").strip().split()
                    
                    record = {
                        'sn': int(row['sn']),
                        'applicationid': int(row['appid']),
                        'name': row['name'],
                        'gender': row['gender'],
                        'district': rest_dict['district'].strip(),
                        'ioe_rank': int(rest_dict['rank']),
                        'quota': rest_dict['quota'].strip() or 'None'
                    }
                    
                    # Map choices p1 to p14
                    for i in range(1, 15):
                        record[f'p{i}'] = int(prefs_raw[i-1]) if i-1 < len(prefs_raw) else None
                        
                    records.append(record)

    df = pd.DataFrame(records)
    
    # 1. Filter all candidates with IOE Rank <= 1010 (no preference condition)
    df_filtered = df[df['ioe_rank'] <= 1010].copy()
    
    # 2. Sort in ascending order by IOE Rank
    df_sorted = df_filtered.sort_values(by='ioe_rank', ascending=True).reset_index(drop=True)
    
    # 3. Write output to text file
    with open(output_filepath, 'w', encoding='utf-8') as f:
        f.write("=" * 150 + "\n")
        f.write("ALL CANDIDATES WITH IOE RANK <= 1010 (SORTED BY IOE RANK ASCENDING)\n")
        f.write("=" * 150 + "\n\n")
        
        # Header layout
        header = f"{'Order':<7}{'SN':<6}{'App ID':<10}{'Name':<24}{'Gender':<8}{'District':<18}{'IOE Rank':<10}"
        header += "".join([f"p{i:<3}" for i in range(1, 15)])
        header += f"{'Quota':<20}\n"
        
        f.write(header)
        f.write("-" * (len(header) - 1) + "\n")
        
        for idx, row in df_sorted.iterrows():
            line_str = f"{idx+1:<7}{row['sn']:<6}{row['applicationid']:<10}{row['name']:<24}{row['gender']:<8}{row['district']:<18}{row['ioe_rank']:<10}"
            
            # Format p1 to p14
            for i in range(1, 15):
                val = str(row[f'p{i}']) if pd.notnull(row[f'p{i}']) else '-'
                line_str += f"{val:<4}"
                
            line_str += f"{row['quota']:<20}\n"
            f.write(line_str)

    print(f"File created successfully: {output_filepath}")

# Run parser
export_all_up_to_rank1010("2082_thaplist.txt", "2082_all_candidates_rank1010.txt")