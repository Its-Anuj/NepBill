async function searchContactForms()
{
    const query = {
        phoneNumber:
            document.getElementById("phoneFilter").value,

        type:
            document.getElementById("typeFilter").value,

        handled:
            document.getElementById("handledFilter").value,

        createdAfter:
            document.getElementById("createdAfter").value,

        createdBefore:
            document.getElementById("createdBefore").value,

        limit:
            parseInt(document.getElementById("limit").value),

        offset:
            parseInt(document.getElementById("offset").value)
    };

    const response = await fetch("/api/admin/contactforms/query",{
        method:"POST",

        headers:{
            "Content-Type":"application/json"
        },

        body:JSON.stringify(query)
    });

    const forms = await response.json();

    populateTable(forms);
}