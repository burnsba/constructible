CREATE PROCEDURE `consolidate_points`(IN epsilon DOUBLE)
BEGIN

    DECLARE done TINYINT DEFAULT FALSE;
    DECLARE d1x DOUBLE;
    DECLARE d1y DOUBLE;
    DECLARE d1id BIGINT;
    DECLARE d2x DOUBLE;
    DECLARE d2y DOUBLE;
    DECLARE d2id BIGINT;
    DECLARE dx DOUBLE;

    DECLARE cursx
        CURSOR FOR
            SELECT Id,X,Y FROM (SELECT Id, CAST(X AS DOUBLE) AS X, CAST(Y AS DOUBLE) AS Y FROM points_known) a ORDER BY X,Y;

    DECLARE cursy
        CURSOR FOR
            SELECT Id,X,Y FROM (SELECT Id, CAST(X AS DOUBLE) AS X, CAST(Y AS DOUBLE) AS Y FROM points_known) a ORDER BY Y,X;

    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;

    OPEN cursx;
    FETCH NEXT FROM cursx INTO d1id,d1x,d1y;
    my_loopx:
    LOOP
        FETCH NEXT FROM cursx INTO d2id,d2x,d2y;
        IF done THEN
            LEAVE my_loopx;
        ELSE
            SET dx = (SELECT (d2x-d1x)*(d2x-d1x)+(d2y-d1y)*(d2y-d1y));
            IF dx < epsilon THEN
                -- SELECT dx,d1id,d1x,d1y,d2id,d2x,d2y;
                DELETE FROM points_known WHERE Id=d2id;
            ELSE
                SET d1id = (SELECT d2id);
                SET d1x = (SELECT d2x);
                SET d1y = (SELECT d2y);
            END IF;
        END IF;
    END LOOP;
    CLOSE cursx;

    OPEN cursy;
    FETCH NEXT FROM cursy INTO d1id,d1x,d1y;
    my_loopy:
    LOOP
        FETCH NEXT FROM cursy INTO d2id,d2x,d2y;
        IF done THEN
            LEAVE my_loopy;
        ELSE
            SET dx = (SELECT (d2x-d1x)*(d2x-d1x)+(d2y-d1y)*(d2y-d1y));
            IF dx < epsilon THEN
                -- SELECT d1id,d1x,d1y,d2id,d2x,d2y;
                DELETE FROM points_known WHERE Id=d2id;
            ELSE            
                SET d1id = (SELECT d2id);
                SET d1x = (SELECT d2x);
                SET d1y = (SELECT d2y);
            END IF;
        END IF;
    END LOOP;
    CLOSE cursy;
END;