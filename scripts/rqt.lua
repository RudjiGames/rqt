--
-- Copyright 2025 Milos Tosic. All rights reserved.  
-- License: http://www.opensource.org/licenses/BSD-2-Clause
-- 

function projectDependencies_rqt()
	return { "rdebug", "rbase" }
end

function projectAdd_rqt()
	addProject_qt("rqt", true)
end
