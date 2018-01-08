import java.util.Scanner;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Formatter;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.ResultSet;

public class DbFunctions {
//    public static void main(String args[]) throws FileNotFoundException {
//        File outFile = new File("products.csv");
//        Formatter output = new Formatter(outFile);
//        output.format("Vendor Num, Vendor Contact, Vendor State\n");
//        Connection con = null;
//        try { 
//            Class.forName("com.mysql.jdbc.Driver").newInstance();
//            String uid = "root";
//            String pwd = "Moof";
//            con = DriverManager.getConnection("jdbc:mysql:///aviationCo", uid, pwd);
//            if(!con.isClosed())
//                System.out.println("Connected to MySQL server.");
//            Statement statement = null;
//            statement = con.createStatement();
//            // Result set, get the result of the SQL query
//            ResultSet resultSet = null;
//            System.out.println("Testing info functions");
//   
//            employeeInfo("101", resultSet, statement, output);
//            charterInfo("10001", resultSet, statement, output);
//            customerInfo("10010", resultSet, statement, output);
//            
//            
//        } catch (Exception e) {
//            System.err.println("Exception: " + e.getMessage());
//        }
//        output.close();
//  }

    
    public static String employeeInfo(String empNum, ResultSet set, Statement st) {
        String query = "SELECT e.* FROM aviationCo.employee e WHERE e.EMP_NUM = " + empNum;
        String retString = null;
        try {
            set = st.executeQuery(query);
            while(set.next()) {
                String eNum = set.getString("e.EMP_NUM");
                String eFnm = set.getString("e.EMP_FNAME");
                String eLnm = set.getString("e.EMP_LNAME");
                retString = eNum + ", " + eFnm + ", " + eLnm;
                System.out.println(eNum + ", " + eFnm + ", " + eLnm);
            }
        }
        catch (Exception e) {
            System.err.println("Exception: " + e.getMessage());
        }
        return retString;
    }
    
    public static String charterInfo(String chNum, ResultSet set, Statement st) {
        String query = "SELECT ch.* FROM aviationCo.charter ch WHERE ch.CHAR_TRIP = " + chNum;
        String retString = null;
        try {
            set = st.executeQuery(query);
            while(set.next()) {
                String chTr = set.getString("ch.CHAR_TRIP");
                String chDt = set.getString("ch.CHAR_DATE");
                String chDs = set.getString("ch.CHAR_DESTINATION");
                retString = chTr + ", " + chDt + ", " + chDs;
                System.out.println(chTr + ", " + chDt + ", " + chDs);            
            }
        }
        catch (Exception e) {
            System.err.println("Exception: " + e.getMessage());
        }
        return retString;
    }
    
     public static String customerInfo(String custNum, ResultSet set, Statement st) {
        String query = "SELECT c.* FROM aviationCo.customer c WHERE c.CUS_CODE = " + custNum;
        String retString = null;
        try {
            set = st.executeQuery(query);
            while(set.next()) {
                String cusNm = set.getString("c.CUS_CODE");
                String cusLn = set.getString("c.CUS_LNAME");
                String cusFn = set.getString("c.CUS_FNAME");
                String cusBl = set.getString("c.CUS_BALANCE");
                retString = cusNm + ", " + cusLn + ", " + cusFn + ", " + cusBl;
                System.out.println(cusNm + ", " + cusLn + ", " + cusFn + ", " + cusBl);            
            }
        }
        catch (Exception e) {
            System.err.println("Exception: " + e.getMessage());
        }
        return retString;
     }
}