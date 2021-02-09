document.addEventListener("DOMContentLoaded", function(ev) { 
    const searchResultsFrame = document.getElementById("MSearchResults")

    const navTable = document.getElementById("projectalign").parentElement

    const themeToggle = document.createElement("input")
    themeToggle.setAttribute("type", "checkbox")
    themeToggle.className = "themeToggle"
    themeToggle.addEventListener("change", function(e) {
        if (e.target.checked) {
            document.documentElement.setAttribute('data-theme', 'dark')
            searchResultsFrame.contentDocument.documentElement.setAttribute("data-theme", "dark")
            localStorage.setItem('theme', 'dark')
        }
        else {
            document.documentElement.setAttribute('data-theme', 'light') 
            searchResultsFrame.contentDocument.documentElement.setAttribute("data-theme", "light")
            localStorage.setItem('theme', 'light') 
        }
    })

    const themeTd = document.createElement("td")
    themeTd.appendChild(themeToggle)
    navTable.insertBefore(themeTd, navTable.children[1])

    const startTheme = localStorage.getItem('theme')

    if (startTheme) {
        document.documentElement.setAttribute('data-theme', startTheme)
        searchResultsFrame.contentDocument.documentElement.setAttribute("data-theme", startTheme)
        if (startTheme === 'dark')
            themeToggle.checked = true
    }

    searchResultsFrame.onload = function(e) {
        const cssFile  = document.createElement("link")
        cssFile.rel  = "stylesheet"
        cssFile.type = "text/css"
        cssFile.href = "../customsearch.css"

        searchResultsFrame.contentDocument.head.appendChild(cssFile)
        searchResultsFrame.contentDocument.documentElement.setAttribute("data-theme", localStorage.getItem('theme'))
    }

})