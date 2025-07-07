

# DNS Resolver: Iterative vs. Recursive Lookup

This DNS resolver code implements two different methods for resolving domain names: **iterative** and **recursive** lookups. Here are the changes that we have made in the code :


----


## Changes Made

 1. **`send_dns_query` Function :**
- Implemented the **UDP query** using `dns.query.udp(query, server, timeout=TIMEOUT)`.  
  - This allows sending a DNS request over **UDP**, the most common transport protocol for DNS queries.
- Added **error handling** for timeouts and other exceptions.  
  - Ensures the program doesn't crash if a DNS server is unresponsive or if an unexpected issue occurs.

 2. **`extract_next_nameservers` Function :**
- Implemented the resolution of **NS hostnames to IP addresses** using `dns.resolver.Resolver()`.  
  - Nameserver (NS) records often contain hostnames, not direct IPs.  
  - This step converts NS hostnames into actual IP addresses so they can be queried directly.
- Added **error handling** for various DNS resolution issues.  
  - Handles failures like unresolvable NS records, ensuring smooth progression through the lookup process.

 3. **`iterative_dns_lookup` Function :**
- Implemented the **stage progression** from **ROOT → TLD → AUTH**.  
  - The function now follows the proper **iterative** lookup flow, querying each level in order:
    1. **Root Servers** → Get the TLD (Top-Level Domain) server.  
    2. **TLD Servers** → Get the authoritative nameserver.  
    3. **Authoritative Server** → Get the final IP address of the domain.  
- Added logic to **remove failed servers** from the list of nameservers.  
  - If a server doesn't respond, it's removed from the list to prevent retries on an unresponsive server.

 4. **`recursive_dns_lookup` Function :**
- Implemented **recursive resolution** using `dns.resolver.resolve()`.  
  - This function now fully delegates the query to a recursive DNS resolver, which completes all lookups internally.
- Added **resolution and printing of nameservers** (NS records).  
  - Now retrieves **both A records (IP addresses) and NS records (nameservers)** to give a full picture of the DNS resolution process.
- Improved **error handling** for various DNS resolution scenarios.  
  - Handles issues like:
    - No response from the resolver.
    - Non-existent domains.
    - Invalid record formats.

 5. **`print_usage` Function :**
- Added a **new function** to display usage instructions.  
  - Provides clear guidance on how to run the script and use different lookup modes.
  - Helps users understand command-line arguments without needing to check the code.

 6. **Main Execution Block :**
- Added **argument checking** and **usage display**.  
  - Ensures the user provides the correct inputs before proceeding with the lookup.
- Implemented **mode selection** based on command-line arguments.  
  - The user can now **choose between iterative or recursive lookup** directly from the command line.
  - Prevents execution errors if no valid mode is selected.
---

## How to Execute

To run the DNS resolver, use the following command:

```sh
python3 dns_sol.py <mode> <domain> 
```
Where:

- `<mode>` is either `"iterative"` or `"recursive"`.  
- `<domain>` is the domain name you want to resolve (e.g., `google.com`).  

### Examples:

#### For iterative lookup:
```sh
python3 dns_sol.py iterative google.com
```
#### For recursive lookup:
For recursive lookup:
``` sh
python3 dns_sol.py recursive google.com
```
---

### Sources :

- dnspython Documentation: https://dnspython.readthedocs.io/

- RFC 1034 and RFC 1035: https://datatracker.ietf.org/doc/html/rfc1034, https://datatracker.ietf.org/doc/html/rfc1035

- IANA Root Server Information: https://www.iana.org/domains/root/servers

- Python Standard Library Documentation: https://docs.python.org/3/library/



- dnspython GitHub Repository: https://github.com/rthalley/dnspython

----

### **Individual Contributions**:
- Himanshu Mahale (230476) :
   design, implementation, research
- Vineet Nagrale(231158) : readme, testing.

---
## Declaration

We declare that we have not indulged in plagiarism

---