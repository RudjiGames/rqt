--
-- Copyright 2025 Milos Tosic. All rights reserved.  
-- License: http://www.opensource.org/licenses/BSD-2-Clause
-- 

function projectDependencies_rqt()
	return { "rdebug", "rbase" }
end

function projectExtraConfig_rqt() 
	includedirs	{ path.join(projectGetPath("rqt"), "../") }
end

function projectExtraConfigExecutable_rqt() 
	includedirs	{ path.join(projectGetPath("rqt"), "../") }
end

function projectAdd_rqt()
	addProject_qt("rqt", LibraryType.Tool)
end
