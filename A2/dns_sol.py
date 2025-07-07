import dns.message
import dns.query
import dns.rdatatype
import dns.resolver
import time
import sys

# Root DNS servers used to start the iterative resolution process
# we have added a number of extra lines to the code so that it handles various kind of errors that might occur during the DNS resolution process. This includes timeouts, server unavailability, and other exceptions that might arise. The code now prints out specific error messages for each type of issue, making it easier to understand what went wrong during the resolution process.
# we have kept the comments from the original source code,added new comments to explain the changes we made to the code and also longer comments as specified in the assignment.
ROOT_SERVERS = {
    "198.41.0.4": "Root (a.root-servers.net)",
    "199.9.14.201": "Root (b.root-servers.net)",
    "192.33.4.12": "Root (c.root-servers.net)",
    "199.7.91.13": "Root (d.root-servers.net)",
    "192.203.230.10": "Root (e.root-servers.net)"
}

TIMEOUT = 3  # Timeout in seconds for each DNS query attempt
# This code defines a TIMEOUT constant, which represents the maximum time (in seconds) allowed for each DNS query attempt. If a query takes longer than this timeout, the code will print an error message indicating that the query timed out. This helps prevent the script from getting stuck indefinitely if a server is unresponsive or slow to respond.
def send_dns_query(server, domain):
    """ 
    Sends a DNS query to the given server for an A record of the specified domain.
    Returns the response if successful, otherwise returns None.
    """

#This code sends a DNS query over UDP, returns the response if successful and handles timeouts or other exceptions by printing an error message and returning None 

    try:
        query = dns.message.make_query(domain, dns.rdatatype.A)  # Construct the DNS query
        response = dns.query.udp(query, server, timeout=TIMEOUT)  # Send query over UDP
        return response
    except dns.exception.Timeout:
        print(f"[ERROR] Query to {server} timed out.")
        return None
    except Exception as e:
        print(f"[ERROR] Query to {server} failed: {e}")
        return None
# This code defines a function called extract_next_nameservers that takes a DNS response object as input and extracts the nameserver (NS) records from the authority section of the response. It then resolves the nameserver domain names to IP addresses using the dns.resolver module. The resolved IP addresses are stored in a list and returned at the end of the function.
def extract_next_nameservers(response):
    """ 
    Extracts nameserver (NS) records from the authority section of the response.
    Then, resolves those NS names to IP addresses.
    Returns a list of IPs of the next authoritative nameservers.
    """
    ns_ips = []  # List to store resolved nameserver IPs
    ns_names = []  # List to store nameserver domain names
    
    # Loop through the authority section to extract NS records
    # This code extracts the nameserver (NS) records from the authority section of the DNS response. It then resolves the nameserver domain names to IP addresses using the dns.resolver module. The resolved IP addresses are stored in a list and returned at the end of the function.
    for rrset in response.authority:
        if rrset.rdtype == dns.rdatatype.NS:
            for rr in rrset: # Iterate over NS records
                ns_name = rr.to_text()
                ns_names.append(ns_name)  # Extract nameserver hostname
                print(f"Extracted NS hostname : {ns_name}")
    

    # Resolve the extracted NS hostnames to IP addresses
    # This code resolves the nameservers extracted from the authority section of the response to IP addresses. It uses the dns.resolver module to perform the resolution and handles different types of exceptions that might occur during the process. The resolved IP addresses are stored in a list and returned at the end of the function.
    resolver = dns.resolver.Resolver()  # This code takes the names of nameservers and converts them into IP addresses. It checks each one, handles any issues that come up (like if the server doesn't exist or the process times out), and prints out the results or any error messages.
    for ns_name in ns_names:
        try:
            answer = resolver.resolve(ns_name, 'A')
            for rdata in answer:
                ns_ip = rdata.to_text()
                ns_ips.append(ns_ip)
                print(f"Resolved {ns_name} to {ns_ip}")
        # Handle resolution errors        
        except dns.resolver.NXDOMAIN:
            print(f"[ERROR] Nameserver {ns_name} does not exist.")
        except dns.resolver.Timeout:

            print(f"[ERROR] Resolution of {ns_name} timed out.")
        except Exception as e:
            print(f"[ERROR] Failed to resolve {ns_name}: {e}")
    
    return ns_ips  # Return list of resolved nameserver IPs

def iterative_dns_lookup(domain): # This code defines a function called iterative_dns_lookup that takes a domain name as input and performs an iterative DNS resolution starting from the root servers. It queries root servers, then TLD servers, then authoritative servers, following the hierarchy until an answer is found or resolution fails.
    """ 
    Performs an iterative DNS resolution starting from root servers.
    It queries root servers, then TLD servers, then authoritative servers,
    following the hierarchy until an answer is found or resolution fails.
    """
    print(f"[ Iterative DNS Lookup ] Resolving {domain}")

    next_ns_list = list(ROOT_SERVERS.keys())  # Start with the root server IPs
    stage = "ROOT"  # Track resolution stage (ROOT, TLD, AUTH)

    while next_ns_list:
        ns_ip = next_ns_list[0]  # Pick the first available nameserver to query
        response = send_dns_query(ns_ip, domain)
        
        if response:  # Check if response is not None
            print(f"[ DEBUG ] Querying {stage} server ({ns_ip}) - SUCCESS")
            
            # If an answer is found, print and return
            if response.answer:
                print(f"[ SUCCESS ] {domain} -> {response.answer[0][0]}")
                return
            
            # If no answer, extract the next set of nameservers
            next_ns_list = extract_next_nameservers(response)

# This code manages the progression through different stages of DNS resolution. It moves from ROOT to TLD (Top-Level Domain) to AUTH (Authoritative) servers. If a query fails, it removes that server from the list and tries the next one.          
            # Move to the next resolution stage
            if stage == "ROOT":
                stage = "TLD"
            elif stage == "TLD":
                stage = "AUTH"
        else:
            print(f"[ERROR] Query failed for {stage} server {ns_ip}")
            next_ns_list.pop(0)  # Remove the failed server from the list
    
    print("[ERROR] Resolution failed.")  # Final failure message if no nameservers respond

def recursive_dns_lookup(domain): # This code defines a function called recursive_dns_lookup that takes a domain name as input and performs recursive DNS resolution using the system's default resolver. This approach relies on a resolver (like Google DNS or a local ISP resolver) to fetch the result recursively.
    """ 
    Performs recursive DNS resolution using the system's default resolver.
    This approach relies on a resolver (like Google DNS or a local ISP resolver)
    to fetch the result recursively.
    """
    print(f"[ Recursive DNS Lookup ] Resolving {domain}")
    try:
        # Perform recursive resolution using the system's DNS resolver
        answer = dns.resolver.resolve(domain, "A")

# This code looks up and displays the nameservers for a domain, then shows the IP addresses associated with it. If anything goes wrong - like no nameservers found, the domain doesn't exist, or the lookup takes too long - it prints out a specific error message explaining what happened.

        # Resolve and print nameservers
        ns_answer = dns.resolver.resolve(domain, "NS") # Resolve NS records for the domain
        for ns_rdata in ns_answer:
            ns_name = ns_rdata.to_text()
            print(f"[ SUCCESS ] {domain} -> {ns_name}")

# This function provides instructions on how to use the script. It tells users they need to run the script with two pieces of information: the mode (either 'iterative' or 'recursive') and the domain name they want to look up (like google.com). It's like a quick guide for anyone using the program.
        
        # Print the final IP address(es)
        for rdata in answer:
            print(f"[ SUCCESS ] {domain} -> {rdata}")
    except dns.resolver.NoNameservers: # Handle missing nameservers
        print(f"[ERROR] No nameservers available for {domain}")
    except dns.resolver.NXDOMAIN: # Handle non-existent domain
        print(f"[ERROR] Domain {domain} does not exist")
    except dns.resolver.Timeout: # Handle resolution timeout
        print(f"[ERROR] Query timed out for {domain}")
    except Exception as e: # Handle other exceptions
        print(f"[ERROR] Recursive lookup failed: {e}")  # Handle resolution failure


if __name__ == "__main__":
    # Check command-line arguments

# This code checks if the user provided the correct information when running the script. It makes sure there are exactly three arguments (the script name, the mode, and the domain) and that the mode is either "iterative" or "recursive". If these conditions aren't met, it shows the usage instructions to help the user run the script correctly.

    if len(sys.argv) != 3 or sys.argv[1] not in {"iterative", "recursive"}:
        
        print("Usage: python3 dns_server.py <iterative|recursive> <domain>")
        sys.exit(1)

    mode = sys.argv[1]  # Get mode (iterative or recursive)
    domain = sys.argv[2]  # Get domain to resolve
    start_time = time.time()  # Record start time
    
    # Execute the selected DNS resolution mode
    if mode == "iterative":
        iterative_dns_lookup(domain)
    else:
        recursive_dns_lookup(domain)
    
    # This line calculates and displays how long the script took to run. It subtracts the start time from the current time and formats the result to show three decimal places. It's like a stopwatch for the script, telling you how many seconds it took to complete its task.
    
    print(f"Time taken : {time.time() - start_time:.3f} seconds")  # Print execution time